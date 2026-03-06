#ifndef DRAGDROPLIST_H
#define DRAGDROPLIST_H

#include <QListWidget>

class DragDropList : public QListWidget
{
    Q_OBJECT

public:
    explicit DragDropList(QWidget *parent = nullptr);

signals:
    void filesDropped(const QStringList& paths);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // DRAGDROPLIST_H
