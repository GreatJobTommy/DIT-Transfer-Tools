#include "DragDropList.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QDebug>

DragDropList::DragDropList(QWidget *parent)
    : QListWidget(parent) {
    setAcceptDrops(true);
    setDragEnabled(true);
    setDefaultDropAction(Qt::MoveAction);
    viewport()->setAcceptDrops(true);
}

void DragDropList::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        QListWidget::dragEnterEvent(event);
    }
}

void DragDropList::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QStringList paths;
        for (const QUrl &url : event->mimeData()->urls()) {
            QString path = url.toLocalFile();
            if (!path.isEmpty()) {
                paths << path;
            }
        }
        if (!paths.isEmpty()) {
            emit filesDropped(paths);
        }
        event->acceptProposedAction();
    } else {
        QListWidget::dropEvent(event);
    }
}
