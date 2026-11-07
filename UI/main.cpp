#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QUrl>
#include <QCheckBox>
#include <QSpinBox> // поле для выбора целого числа с кнопками увеличения и уменьшения.
#include <QDoubleSpinBox>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QWidget window;
	window.setWindowTitle("Kea");
	window.resize(900, 600);
	window.setStyleSheet("background-color: white;");

	QLabel label;
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

	QCheckBox permissionCheck("У меня есть разрешение на проверку");
	permissionCheck.setStyleSheet("color: black;");

	QPushButton scanButton("Начать проверку");
	scanButton.setStyleSheet(button.styleSheet());

	QLabel depthLabel("Глубина обхода:");
	depthLabel.setStyleSheet("color: black;");

	QSpinBox depthInput;
	depthInput.setRange(0, 5);
	depthInput.setValue(2);
	depthInput.setFixedWidth(100);
	depthInput.setStyleSheet(inputStyle);

	QLabel pageLimitLabel("Максимум страниц:");
	pageLimitLabel.setStyleSheet("color: black;");

	QSpinBox pageLimitInput;
	pageLimitInput.setRange(1, 1000);
	pageLimitInput.setValue(100);
	pageLimitInput.setFixedWidth(100);
	pageLimitInput.setStyleSheet(inputStyle);

	QLabel timeoutLabel("Тайм-аут запроса, секунд:");
	timeoutLabel.setStyleSheet("color: black;");

	QSpinBox timeoutInput;
	timeoutInput.setRange(1, 60);
	timeoutInput.setValue(10);
	timeoutInput.setFixedWidth(100);
	timeoutInput.setStyleSheet(inputStyle);

	QLabel rateLabel("Запросов в секунду:");
	rateLabel.setStyleSheet("color: black;");

	QDoubleSpinBox rateInput;
	rateInput.setRange(0.1, 5.0);
	rateInput.setDecimals(1);
	rateInput.setSingleStep(0.1);
	rateInput.setValue(1.0);
	rateInput.setFixedWidth(100);
	rateInput.setStyleSheet(inputStyle);

	QVBoxLayout layout(&window); // Вертикальный компоновщик, управляет расположением внутри окна
	layout.addStretch();
	layout.addWidget(&button, 0, Qt::AlignCenter); // Добавляет кнопку в компоновщик
	layout.addWidget(&label, 0, Qt::AlignCenter);
	layout.addWidget(&nameInput, 0, Qt::AlignCenter);
	layout.addWidget(&urlInput, 0, Qt::AlignCenter);
	layout.addWidget(&button, 0, Qt::AlignCenter);
	layout.addWidget(&targetList, 0, Qt::AlignCenter);
	layout.addWidget(&deleteButton, 0, Qt::AlignCenter);
	layout.addWidget(&permissionCheck, 0, Qt::AlignCenter);
	layout.addWidget(&scanButton, 0, Qt::AlignCenter);
	layout.addWidget(&depthLabel, 0, Qt::AlignCenter);
	layout.addWidget(&depthInput, 0, Qt::AlignCenter);
	layout.addWidget(&pageLimitLabel, 0, Qt::AlignCenter);
	layout.addWidget(&pageLimitInput, 0, Qt::AlignCenter);
	layout.addWidget(&timeoutLabel, 0, Qt::AlignCenter);
	layout.addWidget(&timeoutInput, 0, Qt::AlignCenter);
	layout.addWidget(&rateLabel, 0, Qt::AlignCenter);
	layout.addWidget(&rateInput, 0, Qt::AlignCenter);
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
		// label.setText("Цель: " + name + "\nАдрес: " + displayUrl);

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

	QObject::connect(&scanButton, &QPushButton::clicked, &window, [&targetList, &permissionCheck, &label, &depthInput, &pageLimitInput, &timeoutInput, &rateInput]() {
		if (targetList.selectedItems().isEmpty()) {
			label.setText("Сначала выберите цель в списке.");
			return;
		}

		if (!permissionCheck.isChecked()) {
			label.setText("Подтвердите наличие разрешения на проверку.");
			return;
		}

		int depth = depthInput.value();
		int pageLimit = pageLimitInput.value();
		int timeout = timeoutInput.value();
		double rate = rateInput.value();

		label.setText("Глубина: " + QString::number(depth) + ", максимум страниц: " + QString::number(pageLimit) + ", тайм-аут: " + QString::number(timeout) + " сек., запросов/сек.: " + QString::number(rate, 'f', 1));
  });

	window.show();

	return app.exec();
}