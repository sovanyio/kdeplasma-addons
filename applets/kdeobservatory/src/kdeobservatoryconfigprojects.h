#ifndef KDEOBSERVATORYCONFIGPROJECTS_HEADER
#define KDEOBSERVATORYCONFIGPROJECTS_HEADER

#include <QWidget>

namespace Ui
{
    class KdeObservatoryConfigProjects;
}

class KdeObservatoryConfigProjects : public QWidget
{
    Q_OBJECT
public:
    KdeObservatoryConfigProjects(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~KdeObservatoryConfigProjects();

public Q_SLOTS:
    void on_psbAddProject_clicked();
    void on_psbRemoveProject_clicked();
    void on_psbEditProject_clicked();

private:
    Ui::KdeObservatoryConfigProjects *m_configProjects;
};

#endif
