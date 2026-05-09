#pragma once

#include <QWidget>
#include <QSettings>

class QLineEdit;
class QPushButton;
class QLabel;
class QFrame;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onConvertHijri();
    void onConvertGregorian();
    void onCopyHijriResult();
    void onCopyGregResult();
    void onClearHijri();
    void onClearGreg();
    void onToggleTheme();
    void onToggleAutoCopy();
    void updateTodayDisplay();

private:
    void setupUI();
    void applyTheme(bool dark);
    void loadSettings();
    void saveSettings();
    void setCopyFeedback(QLabel* label);
    void updateAutoCopyBtn();

    QSettings m_settings;
    bool m_dark = true;
    bool m_autoCopy = true;

    // Header
    QLabel* m_titleLabel = nullptr;
    QPushButton* m_themeBtn = nullptr;
    QPushButton* m_autoCopyBtn = nullptr;

    // Hijri → Gregorian card
    QFrame* m_hijriCard = nullptr;
    QLineEdit* m_hijriInput = nullptr;
    QPushButton* m_hijriConvertBtn = nullptr;
    QPushButton* m_hijriCopyBtn = nullptr;
    QPushButton* m_hijriClearBtn = nullptr;
    QLabel* m_hijriResult = nullptr;
    QLabel* m_hijriStatus = nullptr;

    // Gregorian → Hijri card
    QFrame* m_gregCard = nullptr;
    QLineEdit* m_gregInput = nullptr;
    QPushButton* m_gregConvertBtn = nullptr;
    QPushButton* m_gregCopyBtn = nullptr;
    QPushButton* m_gregClearBtn = nullptr;
    QLabel* m_gregResult = nullptr;
    QLabel* m_gregStatus = nullptr;

    // Today's date card
    QFrame* m_todayCard = nullptr;
    QLabel* m_todayGreg = nullptr;
    QLabel* m_todayHijri = nullptr;

    // Months reference card
    QFrame* m_monthsCard = nullptr;

    // Status bar
    QLabel* m_statusBar = nullptr;
};
