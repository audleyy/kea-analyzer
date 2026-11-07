#include <iostream>
#include <curl/curl.h>
#include <cstddef>
#include <string>
#include <sstream>

using namespace std;

bool hasCsp = false;
bool hasNoSniff = false;
bool hasServerHeader = false;
bool hasPowered = false;
size_t cookieHeadersCount = 0;
size_t cookiesWithoutSecure = 0;

size_t checkHeader(char *data, size_t size, size_t count) {
	size_t bytes = size * count;

	try {
		string line(data, bytes);

		if (line.rfind("HTTP/", 0) == 0) {
			cookieHeadersCount = 0;
			cookiesWithoutSecure = 0;
			hasCsp = false;
			hasNoSniff = false;
			hasServerHeader = false;
			hasPowered = false;
		}

		size_t colon = line.find(":");

		if (colon != string::npos) {
			string heading = line.substr(0, colon);

			for (char &elem : heading) {
				if (elem >= 'A' && elem <= 'Z') {
					elem = elem - ('A' - 'a');
				}
			}

			if (heading == "server") {
				string value = line.substr(colon + 1);

				if (value.find_first_not_of(" \t\r\n") != string::npos) {
					hasServerHeader = true;
				}
			}

			if (heading == "x-powered-by") {
				string value = line.substr(colon + 1);

				if (value.find_first_not_of(" \t\r\n") != string::npos) {
					hasPowered = true;
				}
			}

			if (heading == "content-security-policy") {
				hasCsp = true;
			}

			if (heading == "set-cookie") {
				cookieHeadersCount++;

				string cookieLine = line.substr(colon + 1);
				istringstream parts(cookieLine);
				string part;

				getline(parts, part, ';');

				bool hasSecure = false;

				while	(getline(parts, part, ';')) {
					size_t equals = part.find('=');
					string attribute = part.substr(0, equals);

					size_t first = attribute.find_first_not_of(" \t\r\n");
					size_t last = attribute.find_last_not_of(" \t\r\n");

					attribute = attribute.substr(first, last - first + 1);

					for (char &elem : attribute) {
						if (elem >= 'A' && elem <= 'Z') {
							elem = elem - ('A' - 'a');
						}
					}

					if (attribute == "secure") {
						hasSecure = true;
					}
				}

				if (!hasSecure) {
					cookiesWithoutSecure++;
				}
			}

			if (heading == "x-content-type-options") {
				string value = line.substr(colon + 1);

				size_t first = value.find_first_not_of(" \t\r\n");
				size_t last = value.find_last_not_of(" \t\r\n");

				if (first != string::npos) {
					value = value.substr(first, last - first + 1);

					for (char &elem : value) {
						if (elem >= 'A' && elem <= 'Z') {
							elem = elem - ('A' - 'a');
						}
					}

					if (value == "nosniff") {
						hasNoSniff = true;
					}
				}
			}
		}

		return bytes;
	} catch (...) {
		return 0;
	}
}

int main() {
	CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);

	if (result != CURLE_OK) {
		cerr << "Ошибка инициализации" << endl;
		return 1;
	}

	CURL *curl = curl_easy_init(); // Создает сам объект, через который будем выполнять запрос

	if (curl == nullptr) {
		cerr << "Не удалось создать объект" << endl;
		curl_global_cleanup();
		return 1;
	}

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000");
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Огран на время выполнения
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L); // Запрет на перенаправления
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, checkHeader);

	result = curl_easy_perform(curl); // Отправка запроса

	if (result != CURLE_OK) {
		cerr << "Ошибка запроса" << curl_easy_strerror(result) << endl;
	} else {
		long statusCode = 0;
		char *scheme = nullptr;

		double requestTime = 0.0;

		CURLcode infoResult = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
		CURLcode schemeResult = curl_easy_getinfo(curl, CURLINFO_SCHEME, &scheme);
		CURLcode time = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &requestTime);

		if (schemeResult == CURLE_OK && scheme != nullptr) {
			cout << scheme << endl;
		}

		if (time == CURLE_OK) {
			cout << "Время запроса: " << requestTime << " сек" << endl;
		}

		if (infoResult == CURLE_OK) {
			cout << "Status code: " << statusCode << endl;

			char *contentType = nullptr;

			CURLcode typeResult = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);

			if (typeResult == CURLE_OK && contentType != nullptr) {
				cout << "Content-Type: " << contentType << endl;
			} else {
				cout << "Тип содержимого недоступен" << endl;
			}

			if (hasCsp) {
				cout << "Заголовок Content-Security-Policy найден" << endl;
			}

			if (hasNoSniff) {
				cout << "X-Content-Type-Options: nosniff" << endl;
			} 

			if (hasServerHeader) {
				cout << "Заголовок Server найден" << endl;
			}

			if (hasPowered) {
				cout << "Заголовок X-Powered-By найден" << endl;
			}

			cout << "Заголовков Set-Cookie: " << cookieHeadersCount << endl;
			cout << "Cookie без атрибута Secure: " << cookiesWithoutSecure << endl;

		} else {
			cout << "Не удалось получить код ответа" << curl_easy_strerror(infoResult) << endl;
		}
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return 0;
}