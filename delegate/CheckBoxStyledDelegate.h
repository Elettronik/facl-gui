#ifndef CHECKBOXSTYLEDDELEGATE_H
#define CHECKBOXSTYLEDDELEGATE_H

#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QStyledItemDelegate>

class CheckBoxStyledDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CheckBoxStyledDelegate(QObject *parent = 0);
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

};

#endif // CHECKBOXSTYLEDDELEGATE_H
