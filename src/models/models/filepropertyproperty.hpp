#ifndef FILEPROPERTYPROPERTY_H
#define FILEPROPERTYPROPERTY_H

#include "orm/tiny/model.hpp"

#include "models/torrentpreviewablefileproperty.hpp"

class FilePropertyProperty final :
        public Model<FilePropertyProperty, TorrentPreviewableFileProperty>
{
    friend Model;
    using Model::Model;

public:
    /*! Get the previewable file that owns the file property. */
    std::unique_ptr<Relation<FilePropertyProperty, TorrentPreviewableFileProperty>>
    fileProperty()
    {
        return belongsTo<TorrentPreviewableFileProperty>("file_property_id", {},
                                                         __func__);
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "fileProperty")
            relationVisited<TorrentPreviewableFileProperty>();
    }

    /*! The table associated with the model. */
    QString u_table {"file_property_properties"};

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"fileProperty", &FilePropertyProperty::fileProperty},
    };

    /*! The relations to eager load on every query. */
    QVector<WithItem> u_with {
//        {"fileProperty"},
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"fileProperty"};
};

#endif // FILEPROPERTYPROPERTY_H
