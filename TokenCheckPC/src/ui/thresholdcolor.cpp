#include "thresholdcolor.h"
#include "appsettings.h"
#include "theme.h"

namespace ColorResolver {

QColor glmTokenColor(double pct)
{
    if (pct < 0) return Theme::textDim;
    const AppSettings &s = AppSettings::instance();
    int greenT = s.glmGreenThreshold();
    int yellowT = s.glmYellowThreshold();
    if (pct < greenT) return s.glmGreenColor();
    if (pct < yellowT) return s.glmYellowColor();
    return s.glmRedColor();
}

QColor glmTokenColor(int usedPct)
{
    return glmTokenColor(static_cast<double>(usedPct));
}

QColor dsBalanceColor(double balance, const QString &currency)
{
    const AppSettings &s = AppSettings::instance();
    if (currency == "USD")
        return dsBalanceColor(balance, s.dsUsdGreenThreshold(), s.dsUsdYellowThreshold(),
                              s.dsUsdGreenColor(), s.dsUsdYellowColor(), s.dsUsdRedColor());
    return dsBalanceColor(balance, s.dsGreenThreshold(), s.dsYellowThreshold(),
                          s.dsGreenColor(), s.dsYellowColor(), s.dsRedColor());
}

QColor dsBalanceColor(double balance, double greenThreshold, double yellowThreshold,
                      const QColor &greenColor, const QColor &yellowColor, const QColor &redColor)
{
    if (balance > greenThreshold) return greenColor;
    if (balance > yellowThreshold) return yellowColor;
    return redColor;
}

}
