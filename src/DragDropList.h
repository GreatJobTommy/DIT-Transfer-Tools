#ifndef DRAGDROPLIST_H
#define DRAGDROPLIST_H

#include <QListWidget>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

class DragDropList : public QListWidget {
    Q_OBJECT

public:
    DragDropList(QWidget* parent = nullptr) : QListWidget(parent) {
        setAcceptDrops(true);
        setDragEnabled(true);
        setDropIndicatorShown(true);
        setDefaultDropAction(Qt::MoveAction);
    }

protected:
    void dragEnterEvent(QDragEnterEvent* event) override {
        if (event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    }

    void dropEvent(QDropEvent* event) override {
        if (event->mimeData()->hasUrls()) {
            QList<QUrl> urls = event->mimeData()->urls();
            QStringList files;
            for (const QUrl& url : urls) {
                if (url.isLocalFile()) {
                    files.append(url.toLocalFile());
                }
            }
            if (!files.isEmpty()) {
                emit filesDropped(files);
                event->acceptProposedAction();
            }
        } else if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
            // Handle internal drag
            QListWidget::dropEvent(event);
            emit orderChanged();
        } else {
            event->ignore();
        }
    }

signals:
    void filesDropped(const QStringList& files);
    void orderChanged();
};

#endif // DRAGDROPLIST_H