/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include "qdraglistwidget.h"

void QDragListWidget::MoveAll(QDragListWidget *another)
{
	for(int i = 0; i < count(); i++)
	{
		another->addItem(item(i)->text());
	}

	clear();
}

void QDragListWidget::MoveSelected(QDragListWidget *another)
{
	QList<QListWidgetItem*> selected = selectedItems();

	for(QList<QListWidgetItem*>::iterator i = selected.begin(); i != selected.end(); i++)
	{
		another->addItem((*i)->text());
	}

	QStringList currentList;
	for(int i = 0; i < count(); i++)
	{
		currentList << item(i)->text();
	}

	int j;
	for(QList<QListWidgetItem*>::iterator i = selected.begin(); i != selected.end(); i++)
	{
		j = currentList.indexOf((*i)->text());
		if(j >= 0) currentList.removeAt(j);
	}

	clear();
	addItems(currentList);
}

void QDragListWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->source() != this)
		event->acceptProposedAction();
}

void QDragListWidget::dropEvent(QDropEvent *event)
{
	if(event->source() == this) return;

	QList<QUrl> urlList;
	QListWidgetItem *item;
	QFileInfo info;
	QString fName;

	urlList = event->mimeData()->urls();

	foreach(QUrl url, urlList)
	{
		fName = url.toLocalFile();
		info.setFile( fName );
		if(!info.exists() || !info.isFile()) continue;
		item = new QListWidgetItem(info.absoluteFilePath());
		addItem(item);
	}

	QDragListWidget *sourceList = dynamic_cast<QDragListWidget*>(event->source());
	if(!sourceList) return;
	
	/// TODO: przydaloby sie to przepisac z uzyciem MVC, wtedy ponizszy fragment bylby banalny

	QStringList currentList;
	for(int i = 0; i < sourceList->count(); i++)
	{
		currentList << sourceList->item(i)->text();
	}

	QList<QListWidgetItem*> selected = sourceList->selectedItems();
	int j;
	for(QList<QListWidgetItem*>::iterator i = selected.begin(); i != selected.end(); i++)
	{
		j = currentList.indexOf((*i)->text());
		if(j >= 0) currentList.removeAt(j);
	}

	sourceList->clear();
	sourceList->addItems(currentList);

	emit dragFinished();
}

QStringList QDragListWidget::mimeTypes () const 
{
	QStringList list;
	list.append("text/uri-list");
	return list;
}

Qt::DropActions QDragListWidget::supportedDropActions () const
{
	return Qt::CopyAction | Qt::MoveAction;
}

void QDragListWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton)) return;

	if (currentItem() == NULL) return;

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;

	QList<QUrl> list;
	QList<QListWidgetItem *> selected = selectedItems();

	for(QList<QListWidgetItem *>::iterator i = selected.begin(); i != selected.end(); i++)
	{
		list << QUrl((*i)->text());
	}

	mimeData->setUrls(list); 
	drag->setMimeData(mimeData);
	drag->start(Qt::CopyAction | Qt::MoveAction);
}
