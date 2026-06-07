#ifndef THRESHOLDCOLOR_H
#define THRESHOLDCOLOR_H

#include <QColor>
#include "types.h"

class AppSettings;

namespace ColorResolver {

QColor glmTokenColor(double pct);
QColor glmTokenColor(int usedPct);

QColor dsBalanceColor(double balance, const QString &currency);
QColor dsBalanceColor(double balance, double greenThreshold, double yellowThreshold,
                      const QColor &greenColor, const QColor &yellowColor, const QColor &redColor);

}

#endif
