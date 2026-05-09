#include "mainwindow.h"
#include "conversion.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QClipboard>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QDate>
#include <QTimer>
#include <QSizePolicy>

// ============================================================================
// Dark / Light palettes
// ============================================================================
struct ThemeColors {
    QString bg, card, cardHover, border, accent, accentHover, accentText;
    QString textPri, textSec, textMuted, success, error;
    QString inputBg, inputBorder, inputFocus;
    QString btnSecBg, btnSecBorder, btnSecText;
    QString shadow;
    QString todayBg;
};

static ThemeColors darkTheme() {
    return {
        "#0D1117", "#161B22", "#1C2128", "#30363D", "#3B82F6", "#60A5FA", "#0D1117",
        "#E6EDF3", "#8B949E", "#484F58", "#3FB950", "#FF7B72",
        "#010409", "#30363D", "#3B82F6",
        "#21262D", "#363B42", "#C9D1D9",
        "rgba(0,0,0,0.4)", "#1C2128"
    };
}

static ThemeColors lightTheme() {
    return {
        "#F0F4F8", "#FFFFFF", "#F8FAFB", "#D0D7E1", "#2563EB", "#1D4ED8", "#FFFFFF",
        "#1C2433", "#55657A", "#9AA5B4", "#1A7F3F", "#C0392B",
        "#FFFFFF", "#C8D0DC", "#2563EB",
        "#EEF2F7", "#C8D0DC", "#374151",
        "rgba(0,0,0,0.08)", "#EFF6FF"
    };
}

static QString gStyleSheet;

static void buildStyleSheet(bool dark)
{
    ThemeColors t = dark ? darkTheme() : lightTheme();
    gStyleSheet = QString(R"(
        QWidget {
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
            font-size: 13px;
            color: %1;
        }
        QMainWindow, QWidget#centralWidget {
            background-color: %2;
        }
        QFrame#card {
            background-color: %3;
            border: 1px solid %4;
            border-radius: 14px;
        }
        QFrame#todayCard {
            background-color: %18;
            border: 1px solid %4;
            border-radius: 14px;
        }
        QLineEdit {
            background-color: %10;
            border: 2px solid %11;
            border-radius: 10px;
            padding: 10px 14px;
            font-family: "Consolas", "Courier New", monospace;
            font-size: 14px;
            color: %1;
            selection-background-color: %5;
            selection-color: %7;
            min-height: 22px;
        }
        QLineEdit:focus {
            border: 2px solid %12;
        }
        QLineEdit::placeholder {
            color: %9;
        }
        QPushButton#accentBtn {
            background-color: %5;
            color: %7;
            border: none;
            border-radius: 10px;
            padding: 10px 24px;
            font-weight: 600;
            font-size: 13px;
        }
        QPushButton#accentBtn:hover {
            background-color: %6;
        }
        QPushButton#accentBtn:pressed {
            background-color: %5;
            padding: 11px 24px 9px 24px;
        }
        QPushButton#secBtn {
            background-color: %13;
            color: %15;
            border: 1px solid %14;
            border-radius: 10px;
            padding: 9px 20px;
            font-weight: 500;
            font-size: 12px;
        }
        QPushButton#secBtn:hover {
            background-color: %4;
        }
        QPushButton#iconBtn {
            background-color: %13;
            border: 1px solid %14;
            border-radius: 18px;
            padding: 6px 10px;
            font-size: 14px;
            min-width: 36px;
            min-height: 36px;
            max-width: 36px;
            max-height: 36px;
        }
        QPushButton#iconBtn:hover {
            background-color: %4;
            border: 1px solid %5;
        }
        QPushButton#toggleBtnOn {
            background-color: %5;
            color: %7;
            border: none;
            border-radius: 8px;
            padding: 4px 12px;
            font-size: 11px;
            font-weight: 700;
        }
        QPushButton#toggleBtnOff {
            background-color: %13;
            color: %15;
            border: 1px solid %14;
            border-radius: 8px;
            padding: 4px 12px;
            font-size: 11px;
            font-weight: 700;
        }
        QLabel#title {
            font-size: 22px;
            font-weight: 700;
            color: %5;
        }
        QLabel#subtitle {
            font-size: 12px;
            color: %9;
        }
        QLabel#cardTitle {
            font-size: 11px;
            font-weight: 600;
            color: %9;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        QLabel#resultOk {
            font-family: "Consolas", monospace;
            font-size: 15px;
            color: %16;
            padding: 6px 0;
        }
        QLabel#resultErr {
            font-family: "Consolas", monospace;
            font-size: 13px;
            color: %17;
            padding: 6px 0;
        }
        QLabel#todayLabel {
            font-size: 18px;
            font-weight: 600;
            color: %1;
        }
        QLabel#todaySub {
            font-size: 13px;
            color: %8;
        }
        QLabel#monthNum {
            font-family: "Consolas", monospace;
            font-size: 13px;
            color: %5;
            font-weight: 600;
        }
        QLabel#monthName {
            font-family: "Consolas", monospace;
            font-size: 13px;
            color: %1;
        }
        QLabel#statusBar {
            font-size: 11px;
            color: %9;
            padding: 6px 12px;
        }
        QScrollArea {
            border: none;
            background: transparent;
        }
    )").arg(t.textPri, t.bg, t.card, t.border, t.accent, t.accentHover, t.accentText,
             t.textSec, t.textMuted, t.inputBg, t.inputBorder, t.inputFocus,
             t.btnSecBg, t.btnSecBorder, t.btnSecText, t.success, t.error,
             t.todayBg);
}

// ============================================================================
// MainWindow
// ============================================================================
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_settings("Blink", "BlinkDateConverter")
{
    umm_init();
    loadSettings();
    setupUI();
    applyTheme(m_dark);
    setMinimumSize(720, 820);
    resize(720, 900);
    updateTodayDisplay();

    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &MainWindow::updateTodayDisplay);
    t->start(60000);

    int fails = umm_run_tests();
    if (m_statusBar) {
        if (fails == 0)
            m_statusBar->setText("Ready  \u2014  Umm al-Qura  "
                + QString::number(UMM_MIN_YEAR) + "\u2013" + QString::number(UMM_MAX_YEAR) + " AH");
        else
            m_statusBar->setText(QString("Warning: %1 test(s) failed").arg(fails));
    }
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), m_dark ? QColor("#0D1117") : QColor("#F0F4F8"));
}

void MainWindow::resizeEvent(QResizeEvent*)
{
}

static QFrame* makeCard(QWidget* parent, bool dark)
{
    QFrame* f = new QFrame(parent);
    f->setObjectName("card");
    f->setFrameShape(QFrame::NoFrame);
    auto* shadow = new QGraphicsDropShadowEffect(f);
    shadow->setBlurRadius(24);
    shadow->setColor(dark ? QColor(0,0,0,60) : QColor(0,0,0,25));
    shadow->setOffset(0, 4);
    f->setGraphicsEffect(shadow);
    return f;
}

static QFrame* makeTodayCard(QWidget* parent, bool dark)
{
    QFrame* f = new QFrame(parent);
    f->setObjectName("todayCard");
    f->setFrameShape(QFrame::NoFrame);
    auto* shadow = new QGraphicsDropShadowEffect(f);
    shadow->setBlurRadius(24);
    shadow->setColor(dark ? QColor(0,0,0,60) : QColor(0,0,0,25));
    shadow->setOffset(0, 4);
    f->setGraphicsEffect(shadow);
    return f;
}

static QPushButton* makeAccentBtn(QWidget* parent, const QString& text)
{
    QPushButton* b = new QPushButton(text, parent);
    b->setObjectName("accentBtn");
    b->setCursor(Qt::PointingHandCursor);
    b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    b->setMinimumHeight(40);
    return b;
}

static QPushButton* makeSecBtn(QWidget* parent, const QString& text)
{
    QPushButton* b = new QPushButton(text, parent);
    b->setObjectName("secBtn");
    b->setCursor(Qt::PointingHandCursor);
    b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    b->setMinimumHeight(36);
    return b;
}

static QPushButton* makeIconBtn(QWidget* parent, const QString& text)
{
    QPushButton* b = new QPushButton(text, parent);
    b->setObjectName("iconBtn");
    b->setCursor(Qt::PointingHandCursor);
    b->setFixedSize(36, 36);
    return b;
}

static QLabel* makeLabel(QWidget* parent, const QString& text, const QString& objName)
{
    QLabel* l = new QLabel(text, parent);
    l->setObjectName(objName);
    l->setWordWrap(true);
    return l;
}

void MainWindow::setupUI()
{
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 16, 20, 10);
    root->setSpacing(14);

    // =====================================================================
    // HEADER
    // =====================================================================
    QHBoxLayout* header = new QHBoxLayout();
    header->setSpacing(10);

    m_titleLabel = makeLabel(this, "Blink", "title");
    QLabel* dot = makeLabel(this, "\u00B7", "title");
    dot->setStyleSheet("color: " + QString(m_dark?"#484F58":"#9AA5B4") + ";");
    QLabel* sub = makeLabel(this, "Date Converter", "title");
    sub->setStyleSheet("color: " + QString(m_dark?"#E6EDF3":"#0F172A") + ";");

    header->addWidget(m_titleLabel);
    header->addWidget(dot);
    header->addWidget(sub);
    header->addStretch();

    m_autoCopyBtn = new QPushButton(this);
    m_autoCopyBtn->setCursor(Qt::PointingHandCursor);
    m_autoCopyBtn->setFixedHeight(28);
    connect(m_autoCopyBtn, &QPushButton::clicked, this, &MainWindow::onToggleAutoCopy);
    header->addWidget(m_autoCopyBtn);
    header->addSpacing(8);

    m_themeBtn = makeIconBtn(this, m_dark ? "\u2600" : "\u263D");
    connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::onToggleTheme);
    header->addWidget(m_themeBtn);
    root->addLayout(header);

    QLabel* subtitle = makeLabel(this,
        "Saudi Umm al-Qura calendar  \u00B7  "
        + QString::number(UMM_MIN_YEAR) + "\u2013" + QString::number(UMM_MAX_YEAR)
        + " AH  \u00B7  fully offline", "subtitle");
    root->addWidget(subtitle);

    // =====================================================================
    // TODAY CARD
    // =====================================================================
    m_todayCard = makeTodayCard(this, m_dark);
    QVBoxLayout* tcl = new QVBoxLayout(m_todayCard);
    tcl->setContentsMargins(18, 14, 18, 14);
    tcl->setSpacing(8);

    QLabel* tTitle = makeLabel(m_todayCard, "TODAY", "cardTitle");
    tcl->addWidget(tTitle);

    QHBoxLayout* tRow = new QHBoxLayout();
    tRow->setSpacing(20);
    m_todayGreg = makeLabel(m_todayCard, "", "todayLabel");
    m_todayHijri = makeLabel(m_todayCard, "", "todaySub");
    tRow->addWidget(m_todayGreg);
    tRow->addWidget(m_todayHijri);
    tRow->addStretch();
    tcl->addLayout(tRow);
    root->addWidget(m_todayCard);

    // =====================================================================
    // HIJRI -> GREGORIAN CARD
    // =====================================================================
    m_hijriCard = makeCard(this, m_dark);
    QVBoxLayout* hcl = new QVBoxLayout(m_hijriCard);
    hcl->setContentsMargins(18, 14, 18, 14);
    hcl->setSpacing(10);

    QLabel* hTitle = makeLabel(m_hijriCard, "HIJRI TO GREGORIAN", "cardTitle");
    hcl->addWidget(hTitle);

    m_hijriInput = new QLineEdit(m_hijriCard);
    m_hijriInput->setPlaceholderText("e.g. 15/09/1445  or  09/1445  or  1445");
    m_hijriInput->setMinimumHeight(42);
    connect(m_hijriInput, &QLineEdit::returnPressed, this, &MainWindow::onConvertHijri);
    hcl->addWidget(m_hijriInput);

    QHBoxLayout* hBtnRow = new QHBoxLayout();
    hBtnRow->setSpacing(8);
    m_hijriConvertBtn = makeAccentBtn(m_hijriCard, "Convert");
    connect(m_hijriConvertBtn, &QPushButton::clicked, this, &MainWindow::onConvertHijri);
    m_hijriCopyBtn = makeSecBtn(m_hijriCard, "Copy Result");
    connect(m_hijriCopyBtn, &QPushButton::clicked, this, &MainWindow::onCopyHijriResult);
    m_hijriClearBtn = makeSecBtn(m_hijriCard, "Clear");
    connect(m_hijriClearBtn, &QPushButton::clicked, this, &MainWindow::onClearHijri);
    hBtnRow->addWidget(m_hijriConvertBtn);
    hBtnRow->addWidget(m_hijriCopyBtn);
    hBtnRow->addWidget(m_hijriClearBtn);
    hBtnRow->addStretch();
    hcl->addLayout(hBtnRow);

    m_hijriResult = makeLabel(m_hijriCard,
        "Your converted date will appear here\u2026", "resultOk");
    m_hijriResult->setObjectName("resultOk");
    m_hijriResult->setStyleSheet("color: " + QString(m_dark?"#484F58":"#9AA5B4") + ";");
    hcl->addWidget(m_hijriResult);

    m_hijriStatus = makeLabel(m_hijriCard, "", "resultErr");
    m_hijriStatus->setVisible(false);
    hcl->addWidget(m_hijriStatus);
    root->addWidget(m_hijriCard);

    // =====================================================================
    // GREGORIAN -> HIJRI CARD
    // =====================================================================
    m_gregCard = makeCard(this, m_dark);
    QVBoxLayout* gcl = new QVBoxLayout(m_gregCard);
    gcl->setContentsMargins(18, 14, 18, 14);
    gcl->setSpacing(10);

    QLabel* gTitle = makeLabel(m_gregCard, "GREGORIAN TO HIJRI", "cardTitle");
    gcl->addWidget(gTitle);

    m_gregInput = new QLineEdit(m_gregCard);
    m_gregInput->setPlaceholderText("e.g. 2024-03-15  or  15/03/2024  or  03/2024  or  2024");
    m_gregInput->setMinimumHeight(42);
    connect(m_gregInput, &QLineEdit::returnPressed, this, &MainWindow::onConvertGregorian);
    gcl->addWidget(m_gregInput);

    QHBoxLayout* gBtnRow = new QHBoxLayout();
    gBtnRow->setSpacing(8);
    m_gregConvertBtn = makeAccentBtn(m_gregCard, "Convert");
    connect(m_gregConvertBtn, &QPushButton::clicked, this, &MainWindow::onConvertGregorian);
    m_gregCopyBtn = makeSecBtn(m_gregCard, "Copy Result");
    connect(m_gregCopyBtn, &QPushButton::clicked, this, &MainWindow::onCopyGregResult);
    m_gregClearBtn = makeSecBtn(m_gregCard, "Clear");
    connect(m_gregClearBtn, &QPushButton::clicked, this, &MainWindow::onClearGreg);
    gBtnRow->addWidget(m_gregConvertBtn);
    gBtnRow->addWidget(m_gregCopyBtn);
    gBtnRow->addWidget(m_gregClearBtn);
    gBtnRow->addStretch();
    gcl->addLayout(gBtnRow);

    m_gregResult = makeLabel(m_gregCard,
        "Your converted date will appear here\u2026", "resultOk");
    m_gregResult->setObjectName("resultOk");
    m_gregResult->setStyleSheet("color: " + QString(m_dark?"#484F58":"#9AA5B4") + ";");
    gcl->addWidget(m_gregResult);

    m_gregStatus = makeLabel(m_gregCard, "", "resultErr");
    m_gregStatus->setVisible(false);
    gcl->addWidget(m_gregStatus);
    root->addWidget(m_gregCard);

    // =====================================================================
    // HIJRI MONTHS CARD
    // =====================================================================
    m_monthsCard = makeCard(this, m_dark);
    QVBoxLayout* mcl = new QVBoxLayout(m_monthsCard);
    mcl->setContentsMargins(18, 14, 18, 14);
    mcl->setSpacing(10);

    QLabel* mTitle = makeLabel(m_monthsCard, "HIJRI MONTHS", "cardTitle");
    mcl->addWidget(mTitle);

    // 4 columns: num | name | num | name
    QGridLayout* grid = new QGridLayout();
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(8);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);
    for (int i = 1; i <= 12; i++) {
        int row = (i - 1) % 6;
        int colBase = ((i - 1) / 6) * 2;
        QLabel* num = makeLabel(m_monthsCard, QString("%1").arg(i, 2, 10, QChar('0')), "monthNum");
        QLabel* name = makeLabel(m_monthsCard, HijriMonthName(i), "monthName");
        grid->addWidget(num, row, colBase);
        grid->addWidget(name, row, colBase + 1);
    }
    mcl->addLayout(grid);
    root->addWidget(m_monthsCard);

    // =====================================================================
    // STATUS BAR
    // =====================================================================
    m_statusBar = makeLabel(this, "", "statusBar");
    root->addWidget(m_statusBar);

    root->addStretch();

    updateAutoCopyBtn();
}

void MainWindow::applyTheme(bool dark)
{
    m_dark = dark;
    buildStyleSheet(dark);
    setStyleSheet(gStyleSheet);

    for (auto* card : {m_hijriCard, m_gregCard, m_monthsCard}) {
        if (card && card->graphicsEffect()) {
            auto* s = qobject_cast<QGraphicsDropShadowEffect*>(card->graphicsEffect());
            if (s) s->setColor(dark ? QColor(0,0,0,60) : QColor(0,0,0,25));
        }
    }
    if (m_todayCard && m_todayCard->graphicsEffect()) {
        auto* s = qobject_cast<QGraphicsDropShadowEffect*>(m_todayCard->graphicsEffect());
        if (s) s->setColor(dark ? QColor(0,0,0,60) : QColor(0,0,0,25));
    }

    m_themeBtn->setText(dark ? "\u2600" : "\u263D");
    m_themeBtn->setToolTip(dark ? "Switch to Light Mode" : "Switch to Dark Mode");
    updateAutoCopyBtn();
}

void MainWindow::loadSettings()
{
    m_dark = m_settings.value("darkMode", true).toBool();
    m_autoCopy = m_settings.value("autoCopy", true).toBool();
}

void MainWindow::saveSettings()
{
    m_settings.setValue("darkMode", m_dark);
    m_settings.setValue("autoCopy", m_autoCopy);
}

void MainWindow::updateAutoCopyBtn()
{
    if (!m_autoCopyBtn) return;
    m_autoCopyBtn->setObjectName(m_autoCopy ? "toggleBtnOn" : "toggleBtnOff");
    m_autoCopyBtn->setText(m_autoCopy ? "AUTO COPY ON" : "AUTO COPY OFF");
    // Re-apply stylesheet for this button
    if (m_autoCopy) {
        ThemeColors t = m_dark ? darkTheme() : lightTheme();
        m_autoCopyBtn->setStyleSheet(QString("background-color:%1; color:%2; border:none; border-radius:8px; padding:4px 12px; font-size:11px; font-weight:700;")
            .arg(t.accent).arg(t.accentText));
    } else {
        ThemeColors t = m_dark ? darkTheme() : lightTheme();
        m_autoCopyBtn->setStyleSheet(QString("background-color:%1; color:%2; border:1px solid %3; border-radius:8px; padding:4px 12px; font-size:11px; font-weight:700;")
            .arg(t.btnSecBg).arg(t.btnSecText).arg(t.btnSecBorder));
    }
}

void MainWindow::setCopyFeedback(QLabel* label)
{
    QString old = label->text();
    label->setText(old + "   \u2713 Copied!");
    QTimer::singleShot(2000, this, [label, old]() {
        if (label) label->setText(old);
    });
}

// ============================================================================
// Conversion logic helpers
// ============================================================================
static QString trim(const QString& s)
{
    return s.trimmed();
}

static bool parseInt(const QString& s, int& v)
{
    if (s.isEmpty()) return false;
    bool ok;
    v = s.toInt(&ok);
    return ok && v >= 0;
}

static QString fmtGreg(const GregorianDate& g)
{
    return QString("%1 %2 %3").arg(g.day).arg(GregorianMonthName(g.month)).arg(g.year);
}

static QString fmtHijri(const HijriDate& h)
{
    return QString("%1 %2 %3 AH").arg(h.day).arg(HijriMonthName(h.month)).arg(h.year);
}

static void doCopy(const QString& text)
{
    QApplication::clipboard()->setText(text);
}

// ============================================================================
// Hijri -> Gregorian
// ============================================================================
void MainWindow::onConvertHijri()
{
    m_hijriStatus->setVisible(false);
    m_hijriResult->setObjectName("resultOk");

    QString raw = trim(m_hijriInput->text());
    if (raw.isEmpty()) {
        m_hijriResult->setText("Your converted date will appear here\u2026");
        m_hijriResult->setStyleSheet("color: " + QString(m_dark?"#484F58":"#9AA5B4") + ";");
        return;
    }

    QChar sep;
    if (raw.contains('/')) sep = '/';
    else if (raw.contains('-')) sep = '-';

    int hy = 0, hm = 0, hd = 0;
    QString resultText, copyText, statusText;
    bool ok = false;

    auto rangeErr = [&](int y) {
        statusText = QString("Year %1 is outside supported range (%2\u2013%3 AH).")
            .arg(y).arg(UMM_MIN_YEAR).arg(UMM_MAX_YEAR);
    };

    do {
        if (sep.isNull()) {
            if (!parseInt(raw, hy)) {
                statusText = "Invalid input.  Use:  yyyy  |  mm/yyyy  |  dd/mm/yyyy";
                break;
            }
            if (hy < UMM_MIN_YEAR || hy > UMM_MAX_YEAR) { rangeErr(hy); break; }
            GregorianDate gs, ge;
            if (!UmmYearToGregorianRange(hy, gs, ge)) {
                statusText = "Conversion error."; break;
            }
            resultText = QString("%1 AH  \u2192  %2  \u2013  %3")
                .arg(hy).arg(fmtGreg(gs)).arg(fmtGreg(ge));
            copyText = fmtGreg(gs) + " \u2013 " + fmtGreg(ge);
            statusText = "Year range converted.";
            ok = true;
            break;
        }

        auto parts = raw.split(sep, Qt::SkipEmptyParts);
        if (parts.size() == 2) {
            if (!parseInt(trim(parts[0]), hm) || !parseInt(trim(parts[1]), hy)) {
                statusText = "Invalid format.  Use:  mm/yyyy  e.g.  09/1445"; break;
            }
            if (hy < UMM_MIN_YEAR || hy > UMM_MAX_YEAR) { rangeErr(hy); break; }
            if (hm < 1 || hm > 12) { statusText = "Month must be 1\u201312."; break; }
            GregorianDate ms, me;
            if (!UmmMonthToGregorianRange(hy, hm, ms, me)) {
                statusText = "Conversion error."; break;
            }
            int mlen = HijriMonthLength(hy, hm);
            resultText = QString("%1 %2 AH  \u2192  %3  \u2013  %4  (%5 days)")
                .arg(HijriMonthName(hm)).arg(hy)
                .arg(fmtGreg(ms)).arg(fmtGreg(me)).arg(mlen);
            copyText = fmtGreg(ms) + " \u2013 " + fmtGreg(me);
            statusText = "Month range converted.";
            ok = true;
        } else if (parts.size() == 3) {
            if (!parseInt(trim(parts[0]), hd) || !parseInt(trim(parts[1]), hm) || !parseInt(trim(parts[2]), hy)) {
                statusText = "Invalid format.  Use:  dd/mm/yyyy  e.g.  15/09/1445"; break;
            }
            if (hy < UMM_MIN_YEAR || hy > UMM_MAX_YEAR) { rangeErr(hy); break; }
            if (hm < 1 || hm > 12) { statusText = "Month must be 1\u201312."; break; }
            int mlen = HijriMonthLength(hy, hm);
            if (hd < 1 || hd > mlen) {
                statusText = QString("Day %1 is invalid \u2014 %2 %3 AH has %4 days.")
                    .arg(hd).arg(HijriMonthName(hm)).arg(hy).arg(mlen);
                break;
            }
            GregorianDate g = HijriToGregorian(hy, hm, hd);
            if (!g.valid) { statusText = "Conversion error."; break; }
            resultText = QString("%1 %2 %3 AH  \u2192  %4")
                .arg(hd).arg(HijriMonthName(hm)).arg(hy).arg(fmtGreg(g));
            copyText = fmtGreg(g);
            statusText = "Date converted.";
            ok = true;
        } else {
            statusText = "Invalid format.  Use:  yyyy  |  mm/yyyy  |  dd/mm/yyyy";
        }
    } while (false);

    m_hijriResult->setText(resultText.isEmpty() ? statusText : resultText);
    m_hijriResult->setStyleSheet(ok
        ? "color: " + QString(m_dark?"#3FB950":"#1A7F3F") + "; font-family: Consolas; font-size: 15px; padding: 6px 0;"
        : "color: " + QString(m_dark?"#FF7B72":"#C0392B") + "; font-family: Consolas; font-size: 13px; padding: 6px 0;");
    m_hijriStatus->setText(statusText);
    m_hijriStatus->setVisible(!ok);
    if (ok && m_autoCopy) {
        m_hijriStatus->setText(statusText + "   \u2713 Copied!");
        m_hijriStatus->setVisible(true);
        m_hijriStatus->setStyleSheet("color: " + QString(m_dark?"#3FB950":"#1A7F3F") + "; font-size: 12px; padding: 4px 0;");
        doCopy(copyText);
    } else if (ok) {
        m_hijriStatus->setText(statusText);
        m_hijriStatus->setVisible(true);
        m_hijriStatus->setStyleSheet("color: " + QString(m_dark?"#8B949E":"#55657A") + "; font-size: 12px; padding: 4px 0;");
    }
    m_hijriInput->clear();
    m_hijriInput->setFocus();
}

// ============================================================================
// Gregorian -> Hijri
// ============================================================================
void MainWindow::onConvertGregorian()
{
    m_gregStatus->setVisible(false);
    QString raw = trim(m_gregInput->text());
    if (raw.isEmpty()) {
        m_gregResult->setText("Your converted date will appear here\u2026");
        m_gregResult->setStyleSheet("color: " + QString(m_dark?"#484F58":"#9AA5B4") + ";");
        return;
    }

    QChar sep;
    if (raw.contains('/')) sep = '/';
    else if (raw.contains('-')) sep = '-';

    int gy = 0, gm = 0, gd = 0;
    QString resultText, copyText, statusText;
    bool ok = false;

    do {
        if (sep.isNull()) {
            // Year only
            if (!parseInt(raw, gy)) {
                statusText = "Invalid format.  Use:  yyyy  |  mm/yyyy  |  dd/mm/yyyy";
                break;
            }
            if (gy < 1900 || gy > 2100) {
                statusText = QString("Year %1 is outside reasonable range.").arg(gy);
                break;
            }
            // Find Hijri year that contains this Gregorian year
            // Try Jan 1 and Dec 31 of this year
            HijriDate h1 = GregorianToHijri(gy, 1, 1);
            HijriDate h2 = GregorianToHijri(gy, 12, 31);
            if (!h1.valid || !h2.valid) {
                statusText = QString("Year %1 is outside Umm al-Qura range.").arg(gy);
                break;
            }
            if (h1.year == h2.year) {
                resultText = QString("%1  \u2192  %2 AH  (full year)")
                    .arg(gy).arg(h1.year);
                copyText = QString("%1 AH").arg(h1.year);
            } else {
                resultText = QString("%1  \u2192  %2 AH  \u2013  %3 AH")
                    .arg(gy).arg(h1.year).arg(h2.year);
                copyText = QString("%1 AH \u2013 %2 AH").arg(h1.year).arg(h2.year);
            }
            statusText = "Year converted.";
            ok = true;
            break;
        }

        auto parts = raw.split(sep, Qt::SkipEmptyParts);
        if (parts.size() == 2) {
            // Month/Year
            if (!parseInt(trim(parts[0]), gm) || !parseInt(trim(parts[1]), gy)) {
                statusText = "Invalid format.  Use:  mm/yyyy  e.g.  03/2024"; break;
            }
            if (gy < 1000) std::swap(gy, gm);
            if (gm < 1 || gm > 12) { statusText = "Month must be 1\u201312."; break; }
            if (gy < 1900 || gy > 2100) { statusText = "Year outside reasonable range."; break; }
            if (!IsValidGregorianDate(gy, gm, 1)) {
                statusText = "Invalid Gregorian date."; break;
            }
            int dim = QDate(gy, gm, 1).daysInMonth();
            HijriDate h1 = GregorianToHijri(gy, gm, 1);
            HijriDate h2 = GregorianToHijri(gy, gm, dim);
            if (!h1.valid || !h2.valid) {
                statusText = QString("Date outside Umm al-Qura range (%1\u2013%2 AH).")
                    .arg(UMM_MIN_YEAR).arg(UMM_MAX_YEAR);
                break;
            }
            if (h1.year == h2.year && h1.month == h2.month) {
                resultText = QString("%1 %2 %3  \u2192  %4  (%5 days)")
                    .arg(GregorianMonthName(gm)).arg(gy)
                    .arg(fmtHijri(h1)).arg(dim);
                copyText = fmtHijri(h1);
            } else {
                resultText = QString("%1 %2  \u2192  %3  \u2013  %4  (%5 days)")
                    .arg(GregorianMonthName(gm)).arg(gy)
                    .arg(fmtHijri(h1)).arg(fmtHijri(h2)).arg(dim);
                copyText = fmtHijri(h1) + " \u2013 " + fmtHijri(h2);
            }
            statusText = "Month converted.";
            ok = true;
        } else if (parts.size() == 3) {
            if (!parseInt(trim(parts[0]), gy) || !parseInt(trim(parts[1]), gm) || !parseInt(trim(parts[2]), gd)) {
                statusText = "Invalid format.  Use:  yyyy-mm-dd  or  dd/mm/yyyy"; break;
            }
            if (gy < 1000) {
                std::swap(gy, gd);
            }
            if (!IsValidGregorianDate(gy, gm, gd)) {
                statusText = "Invalid Gregorian date.";
                break;
            }
            HijriDate h = GregorianToHijri(gy, gm, gd);
            if (!h.valid) {
                statusText = QString("Date outside Umm al-Qura range (%1\u2013%2 AH).")
                    .arg(UMM_MIN_YEAR).arg(UMM_MAX_YEAR);
                break;
            }
            resultText = QString("%1  \u2192  %2")
                .arg(fmtGreg(GregorianDate{gy,gm,gd,true})).arg(fmtHijri(h));
            copyText = fmtHijri(h);
            statusText = "Date converted.";
            ok = true;
        } else {
            statusText = "Invalid format.  Use:  yyyy  |  mm/yyyy  |  dd/mm/yyyy";
        }
    } while (false);

    m_gregResult->setText(resultText.isEmpty() ? statusText : resultText);
    m_gregResult->setStyleSheet(ok
        ? "color: " + QString(m_dark?"#3FB950":"#1A7F3F") + "; font-family: Consolas; font-size: 15px; padding: 6px 0;"
        : "color: " + QString(m_dark?"#FF7B72":"#C0392B") + "; font-family: Consolas; font-size: 13px; padding: 6px 0;");
    m_gregStatus->setText(statusText);
    m_gregStatus->setVisible(!ok);
    if (ok && m_autoCopy) {
        m_gregStatus->setText(statusText + "   \u2713 Copied!");
        m_gregStatus->setVisible(true);
        m_gregStatus->setStyleSheet("color: " + QString(m_dark?"#3FB950":"#1A7F3F") + "; font-size: 12px; padding: 4px 0;");
        doCopy(copyText);
    } else if (ok) {
        m_gregStatus->setText(statusText);
        m_gregStatus->setVisible(true);
        m_gregStatus->setStyleSheet("color: " + QString(m_dark?"#8B949E":"#55657A") + "; font-size: 12px; padding: 4px 0;");
    }
    m_gregInput->clear();
    m_gregInput->setFocus();
}

void MainWindow::onCopyHijriResult()
{
    QString t = m_hijriResult->text();
    if (t.contains("\u2192")) {
        QString copy = t.mid(t.indexOf("\u2192") + 2).trimmed();
        QApplication::clipboard()->setText(copy);
        setCopyFeedback(m_hijriStatus);
        m_hijriStatus->setVisible(true);
    }
}

void MainWindow::onCopyGregResult()
{
    QString t = m_gregResult->text();
    if (t.contains("\u2192")) {
        QString copy = t.mid(t.indexOf("\u2192") + 2).trimmed();
        QApplication::clipboard()->setText(copy);
        setCopyFeedback(m_gregStatus);
        m_gregStatus->setVisible(true);
    }
}

void MainWindow::onClearHijri()
{
    m_hijriInput->clear();
    m_hijriResult->setText("Your converted date will appear here\u2026");
    m_hijriResult->setStyleSheet("color: " + QString(m_dark?"#484F58":"#9AA5B4") + ";");
    m_hijriStatus->setVisible(false);
    m_hijriInput->setFocus();
}

void MainWindow::onClearGreg()
{
    m_gregInput->clear();
    m_gregResult->setText("Your converted date will appear here\u2026");
    m_gregResult->setStyleSheet("color: " + QString(m_dark?"#484F58":"#9AA5B4") + ";");
    m_gregStatus->setVisible(false);
    m_gregInput->setFocus();
}

void MainWindow::onToggleTheme()
{
    m_dark = !m_dark;
    applyTheme(m_dark);
    saveSettings();
    onClearHijri();
    onClearGreg();
}

void MainWindow::onToggleAutoCopy()
{
    m_autoCopy = !m_autoCopy;
    saveSettings();
    updateAutoCopyBtn();
}

void MainWindow::updateTodayDisplay()
{
    QDate today = QDate::currentDate();
    HijriDate h = GregorianToHijri(today.year(), today.month(), today.day());
    QString gregStr = today.toString("dddd, d MMMM yyyy");
    if (h.valid) {
        QString hijStr = fmtHijri(h);
        m_todayGreg->setText(gregStr);
        m_todayHijri->setText("  \u2014  " + hijStr);
    } else {
        m_todayGreg->setText(gregStr);
        m_todayHijri->setText("  \u2014  Outside supported range");
    }
}
