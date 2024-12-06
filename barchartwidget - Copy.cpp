#include "barchartwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QFont>
#include <QDebug>

BarChartWidget::BarChartWidget(const QMap<QString, int> &stats, QWidget *parent)
    : QWidget(parent), stats(stats) {
    this->setWindowTitle("Deliveries by Address");
    this->resize(600, 400);  // Set initial size
}

void BarChartWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    // Set up fonts and pen
    QFont font("Arial", 10);
    painter.setFont(font);

    // Calculate dimensions for the chart
    int margin = 50;
    int chartWidth = width() - 2 * margin;
    int chartHeight = height() - 2 * margin;
    int barWidth = chartWidth / stats.size();

    // Get the maximum value for scaling
    int maxValue = 0;
    for (auto value : stats) {
        if (value > maxValue)
            maxValue = value;
    }

    // Draw the axes
    painter.drawLine(margin, height() - margin, width() - margin, height() - margin); // X-axis
    painter.drawLine(margin, margin, margin, height() - margin); // Y-axis

    // Draw the bars
    int barIndex = 0;
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        // Calculate bar height
        int barHeight = static_cast<int>((static_cast<double>(it.value()) / maxValue) * chartHeight);

        // Draw the bar
        QRect barRect(margin + barIndex * barWidth, height() - margin - barHeight, barWidth - 10, barHeight);
        painter.setBrush(Qt::blue);
        painter.drawRect(barRect);

        // Draw the labels
        painter.drawText(barRect.left() + 5, height() - margin + 20, it.key());  // Address label
        painter.drawText(barRect.left() + 5, barRect.top() - 5, QString::number(it.value()));  // Count label

        ++barIndex;
    }
}
