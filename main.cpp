#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QUrl>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QWidget window;
	window.setWindowTitle("Kea");
	window.resize(900, 600);
	window.setStyleSheet("background-color: white;");

	QLabel label("Цель не указана");
	label.setTextFormat(Qt::PlainText);
	label.setStyleSheet("color: black; background-color: white;");

	QLineEdit nameInput;
	nameInput.setPlaceholderText("Название цели");
	nameInput.setFixedWidth(300);

	QLineEdit urlInput;
	urlInput.setPlaceholderText("Адрес, например https://example.com");
	urlInput.setFixedWidth(300);

	QString inputStyle =
		"background-color: white;"
		"color: black;"
		"border: 1px solid #999999;"
		"padding: 8px;";

	nameInput.setStyleSheet(inputStyle);
	urlInput.setStyleSheet(inputStyle);

	QPushButton button("Добавить цель");
	button.setStyleSheet(
		"background-color: #eeeeee;"
		"color: black;"
		"border: 1px solid #999999;"
		"padding: 8px 16px;"
  );

	QListWidget targetList;
	targetList.setFixedWidth(400);
	targetList.setMaximumHeight(180);
	targetList.setStyleSheet(
		"background-color: white;"
		"color: black;"
		"border: 1px solid #999999;"
	);

	QPushButton deleteButton("Удалить выбранную цель");
	deleteButton.setStyleSheet(button.styleSheet());

	QVBoxLayout layout(&window); // Вертикальный компоновщик, управляет расположением внутри окна
	layout.addStretch();
	layout.addWidget(&button, 0, Qt::AlignCenter); // Добавляет кнопку в компоновщик
	layout.addWidget(&label, 0, Qt::AlignCenter);
	layout.addWidget(&nameInput, 0, Qt::AlignCenter);
	layout.addWidget(&urlInput, 0, Qt::AlignCenter);
	layout.addWidget(&button, 0, Qt::AlignCenter);
	layout.addWidget(&targetList, 0, Qt::AlignCenter);
	layout.addWidget(&deleteButton, 0, Qt::AlignCenter);
	layout.addStretch();

	QObject::connect(&button, &QPushButton::clicked, &window, [&nameInput, &urlInput, &label, &targetList]() {
		QString name = nameInput.text().trimmed();
    QString address = urlInput.text().trimmed();

		if (name.isEmpty()) {
			label.setText("Введите название цели");
			return;
    }

		QUrl url(address, QUrl::StrictMode);

		if (!url.isValid() || url.host().isEmpty() || (url.scheme() != "http" && url.scheme() != "https") || !url.userInfo().isEmpty()) {
			label.setText("Введите корректный HTTP(S)-адрес без логина и пароля");
			return;
		}

		QString displayUrl = url.toDisplayString(
			QUrl::RemoveQuery | QUrl::RemoveFragment // Очищает url от спец символов
		);

		targetList.addItem(name + " — " + displayUrl);
		label.setText("Цель: " + name + "\nАдрес: " + displayUrl);

		nameInput.clear();
		urlInput.clear();
	});

	QObject::connect(&deleteButton, &QPushButton::clicked, &window, [&targetList, &label]() {
		auto selected = targetList.selectedItems();

		if (selected.isEmpty()) {
			label.setText("Сначала выберите цель в списке");
			return;
		}

		int row = targetList.row(selected.first());
		delete targetList.takeItem(row);

		label.setText("Цель удалена");
	});

	window.show();

	return app.exec();
}