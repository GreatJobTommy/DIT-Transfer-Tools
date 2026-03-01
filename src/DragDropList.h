#ifndef DRAGDROPLIST_H
#define DRAGDROPLIST_H

#include <QListWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

class DragDropList : public QListWidget {
    Q_OBJECT
public:
    DragDropList(QWidget* parent = nullptr) : QListWidget(parent) {
        setAcceptDrops(true);
        setDragDropMode(QAbstractItemView::InternalMove);
        setDefaultDropAction(Qt::MoveAction);
    }
protected:
    void dragEnterEvent(QDragEnterEvent* event) override {
        if (event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
            event->acceptProposedAction();
        }
    }
    void dropEvent(QDropEvent* event) override {
        if (event->mimeData()->hasUrls()) {
            QList<QUrl> urls = event->mimeData()->urls();
            QStringList files;
            for (const QUrl& url : urls) {
                if (url.isLocalFile()) {
                    files << url.toLocalFile();
                }
            }
            emit filesDropped(files);
            event->acceptProposedAction();
        } else {
            QListWidget::dropEvent(event);
            emit orderChanged();
        }
    }
signals:
    void filesDropped(const QStringList& files);
    void orderChanged();
};

#endif // DRAGDROPLIST_H