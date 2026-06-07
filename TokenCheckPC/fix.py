import re

files = ['T:/GitHubDemo/TokenCheck/TokenCheckPC/src/ui/tabs/general_tab.cpp', 'T:/GitHubDemo/TokenCheck/TokenCheckPC/src/ui/tabs/ball_tab.cpp']

for file in files:
    with open(file, 'r', encoding='utf-8') as f:
        content = f.read()

    # Change alignment
    content = content.replace('Qt::AlignRight', 'Qt::AlignLeft')

    # Add helper lambda if not present
    if 'auto mkLbl =' not in content:
        content = re.sub(r'(auto \*mainLayout = new QVBoxLayout\(this\);)', r'\1\n    auto mkLbl = [](const QString &text) { auto *l = new QLabel(text); l->setFixedWidth(130); return l; };', content)

    # replace addRow(tr("..."), ...)
    content = re.sub(r'->addRow\((tr\([^)]+\)), ', r'->addRow(mkLbl(\1), ', content)
    # also match addRow("Type:", ...) if not translated
    content = re.sub(r'->addRow\("([^"]+)", ', r'->addRow(mkLbl(tr("\1")), ', content)
    
    # also fix labels that are explicitly instantiated:
    # m_glmGreenLabel = new QLabel(tr("Green:"));
    # we can just find all `new QLabel(tr("..."))` inside ball_tab and we want to set fixed width on them?
    # actually it's easier to just find `m_xxxxLabel = new QLabel(...)` and append `\n    m_xxxxLabel->setFixedWidth(130);`
    
    with open(file, 'w', encoding='utf-8') as f:
        f.write(content)
