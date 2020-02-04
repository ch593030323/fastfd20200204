#ifndef SAAS_GUIDE_ACTIVATETERMINAL_H
#define SAAS_GUIDE_ACTIVATETERMINAL_H

#include <QFrame>

namespace Ui {
class saas_guide_activateTerminal;
}

class saas_guide_activateTerminal : public QFrame
{
    Q_OBJECT

public:
    explicit saas_guide_activateTerminal(QWidget *parent = 0);
    ~saas_guide_activateTerminal();

    bool toTerminalActive();
private:
    Ui::saas_guide_activateTerminal *ui;
};

#endif // SAAS_GUIDE_ACTIVATETERMINAL_H
