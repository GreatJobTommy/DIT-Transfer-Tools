#include "DragDropList.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QDir>

DragDropList::DragDropList(QWidget *parent)
    : QListWidget(parent)
{
    setAcceptDrops(true);
}

void DragDropList::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void DragDropList::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QStringList paths;
        QList<QUrl> urls = mimeData->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                QString path = url.toLocalFile();
                if (QDir(path).exists()) {
                    paths << path;
                }
            }
        }
        if (!paths.isEmpty()) {
            emit filesDropped(paths);
            event->acceptProposedAction();
        }
    }
}