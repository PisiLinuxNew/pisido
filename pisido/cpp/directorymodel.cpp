#include "directorymodel.h"

#include <QDir>


class FileSystemItem
{
public:
    enum ItemType {DIRECTORY, FILE};

    FileSystemItem(
        const QString & path,
        const QList<QVariant> & data,
        ItemType type,
        FileSystemItem * parent = 0)
    {
        itemPath = path;
        itemData = data;
        itemType = type;
        parentItem = parent;
    }
    FileSystemItem(){
        itemPath = QString();
        parentItem = 0;
        itemType = DIRECTORY;
    }
    ~FileSystemItem(){
        qDeleteAll(childItems);
    }
    bool isValid() const {
        return ! itemPath.isEmpty();
    }
    void appendChild(FileSystemItem * item){
        childItems.append(item);
    }
    FileSystemItem * child(int row){
        if(row < 0 || row >= childItems.size())
            return NULL;
        else
            return childItems.at(row);
    }
    int childCount() const {
        return childItems.count();
    }
    int row() const {
        if(parentItem){
            return parentItem->childItems
                    .indexOf(const_cast<FileSystemItem*>(this));
        }
        return 0;
    }
    int columnCount() const {
        return itemData.count();
    }
    QVariant data(int column) const {
        return itemData.value(column);      // if no, default QVariant
    }
    FileSystemItem * parent() const {
        return parentItem;
    }
    ItemType type() const {
        return itemType;
    }
    QString path() const {
        return itemPath;
    }

private:
    QString itemPath;
    QList<QVariant> itemData;               // row info
    QList<FileSystemItem*> childItems;
    ItemType itemType;
    FileSystemItem * parentItem;
};




DirectoryModel::DirectoryModel(const QDir & dir, QObject *parent) :
    QAbstractItemModel(parent),
    rootDir(dir),
    rootItem(NULL)
{
    refresh();
}

DirectoryModel::~DirectoryModel()
{
    if(rootItem)
        delete rootItem;
}

void DirectoryModel::setRootDirectory(const QDir &dir)
{
    rootDir = dir;
    refresh();
}

void DirectoryModel::refresh()
{
    beginResetModel();
    FileSystemItem * oldRootItem = rootItem;

    FileSystemItem * newRootItem = new FileSystemItem(
                rootDir.absolutePath(),
                generateData(rootDir), FileSystemItem::DIRECTORY);
    setupModelData(rootDir, newRootItem);
    rootItem = newRootItem;
    //resetInternalData();
    endResetModel();

    if(oldRootItem)
        delete oldRootItem;
}

void DirectoryModel::clear()
{
    rootDir = QDir("/invaliddirectory");
    refresh();
}

QList<QVariant> DirectoryModel::generateData(const QDir &dir)
{
    QList<QVariant> data;
    data << (dir.dirName().isEmpty() ? QString("/") : dir.dirName());
    data << generatePermission(QFileInfo(dir.absolutePath()));
    data << QString("");
    data << QFileInfo(dir.absolutePath()).symLinkTarget();
    return data;
}

QList<QVariant> DirectoryModel::generateData(const QFile &file)
{
    QList<QVariant> data;
    data << QFileInfo(file.fileName()).fileName();
    data << generatePermission(QFileInfo(file));
    data << file.size();
    data << file.symLinkTarget();
    return data;
}

void DirectoryModel::setupModelData(const QDir &dir, FileSystemItem *parentItem)
{
    if( ! parentItem){
        qDebug("parent null");
        return;
    }
    if( ! dir.exists())
        return;

    QFileInfoList list = dir.entryInfoList(
                QDir::Dirs|QDir::Files|QDir::Hidden|QDir::NoDotAndDotDot,
                QDir::DirsFirst|QDir::Name);
    foreach (QFileInfo fileInfo, list) {
        if(fileInfo.isDir()){
            QDir dir(fileInfo.absolutePath() +
                     QDir::separator() +
                     fileInfo.fileName());
            FileSystemItem * item = new FileSystemItem(
                        dir.absolutePath(),
                        generateData(dir),
                        FileSystemItem::DIRECTORY,
                        parentItem);
            parentItem->appendChild(item);
            setupModelData(dir, item);
        }
        else if (fileInfo.isFile()) {
            parentItem->appendChild(new FileSystemItem(
                                        fileInfo.absoluteFilePath(),
                                        generateData(
                                            QFile(fileInfo.absoluteFilePath())),
                                        FileSystemItem::FILE,
                                        parentItem));
        }
        else{
        }
    }
}

FileSystemItem * DirectoryModel::getItem(const QModelIndex &index) const
{
    if(index.isValid()){
        return static_cast<FileSystemItem*>(index.internalPointer());
    }
    else{
        return rootItem;
    }
}

QModelIndex DirectoryModel::index(int row, int column, const QModelIndex &parent) const
{
    if( ! hasIndex(row, column, parent))
        return QModelIndex();

    FileSystemItem * parentItem = getItem(parent);
    FileSystemItem * childItem = parentItem->child(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex DirectoryModel::parent(const QModelIndex &child) const
{
    if( ! child.isValid())
        return QModelIndex();

    FileSystemItem * childItem = getItem(child);
    FileSystemItem * parentItem = childItem->parent();

    if(parentItem == rootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

Qt::ItemFlags DirectoryModel::flags(const QModelIndex &index) const
{
    if(index.isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return 0;
}

QVariant DirectoryModel::data(const QModelIndex &index, int role) const
{
    if( ! index.isValid())
        return QVariant();

    FileSystemItem * item = getItem(index);

    if(role == Qt::DisplayRole){
        return item->data(index.column());
    }
    else if(role == Qt::DecorationRole){
        if(index.column() == 0){
            if(item->type() == FileSystemItem::DIRECTORY)
                return QIcon(":/images/folder.png");
            else
                return QIcon(":/images/file.png");
        }
    }

//    else if(role == Qt::ToolTipRole){
//        return QString("Tooltip");
//    }
//    else if(role == Qt::StatusTipRole){
//        return QString("StatusTip");
//    }
    return QVariant();
}


QVariant DirectoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole){
        // section -> columns
        if(section == 0){
            return tr("Name");
        }
        else if(section == 1){
            return tr("Permission");
        }
        else if(section == 2){
            return tr("Size");
        }
        else if(section == 3){
            return tr("Symlink");
        }
    }
    return QVariant();
}

int DirectoryModel::rowCount(const QModelIndex &parent) const
{
    FileSystemItem * parentItem = getItem(parent);
    return parentItem->childCount();
}

int DirectoryModel::columnCount(const QModelIndex &parent) const
{
    FileSystemItem * parentItem = getItem(parent);
    return parentItem->columnCount();
}

bool DirectoryModel::isDir(const QModelIndex &index)
{
    if( ! index.isValid())
        return false;
    FileSystemItem * item = getItem(index);
    return item->type() == FileSystemItem::DIRECTORY;
}

bool DirectoryModel::isFile(const QModelIndex &index)
{
    if( ! index.isValid())
        return false;
    FileSystemItem * item = getItem(index);
    return item->type() == FileSystemItem::FILE;
}

QString DirectoryModel::filePath(const QModelIndex &index)
{
    if( ! index.isValid())
        return QString();
    FileSystemItem * item = getItem(index);
    return item->path();
}


QString DirectoryModel::generatePermission(const QFileInfo &fi)
{
    QFile::Permissions p = fi.permissions();
    QString r;
    // first char
    if(fi.isDir()) r += "d";
    else if(fi.isSymLink()) r += "l";
    else r += "-";
    // user
    r += p & QFile::ReadUser ? "r" : "-";
    r += p & QFile::WriteUser ? "w" : "-";
    r += p & QFile::ExeUser ? "x" : "-";
    // group
    r += p & QFile::ReadGroup ? "r" : "-";
    r += p & QFile::WriteGroup ? "w" : "-";
    r += p & QFile::ExeGroup ? "x" : "-";
    // others
    r += p & QFile::ReadOther ? "r" : "-";
    r += p & QFile::WriteOther ? "w" : "-";
    r += p & QFile::ExeOther ? "x" : "-";
    // with numbers
    r += "(";
    r += QString::number(
                (p & (QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner)) >> 12);
    r += QString::number(
                (p & (QFile::ReadUser|QFile::WriteUser|QFile::ExeUser)) >> 8);
    r += QString::number(
                (p & (QFile::ReadGroup|QFile::WriteGroup|QFile::ExeGroup)) >> 4);
    r += QString::number(
                p & (QFile::ReadOther|QFile::WriteOther|QFile::ExeOther));
    r += ")";
    return r;
}





