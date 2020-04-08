#ifndef DIRECTORYMODEL_H
#define DIRECTORYMODEL_H

#include <QAbstractItemModel>

class QDir;
class QFile;
class FileSystemItem;
class QFileInfo;

class DirectoryModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DirectoryModel(const QDir & dir, QObject *parent = 0);
    ~DirectoryModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    bool isDir(const QModelIndex &index);
    bool isFile(const QModelIndex &index);
    QString filePath(const QModelIndex &index);


signals:

public slots:
    void setRootDirectory(const QDir & dir);
    void refresh();
    void clear();

private:
    QDir rootDir;
    FileSystemItem * rootItem;

    QString generatePermission(const QFileInfo & fi);
    void setupModelData(const QDir & dir, FileSystemItem * parentItem);
    QList<QVariant> generateData(const QDir & dir);
    QList<QVariant> generateData(const QFile & dir);
    FileSystemItem * getItem(const QModelIndex & index) const;
};

#endif // DIRECTORYMODEL_H
