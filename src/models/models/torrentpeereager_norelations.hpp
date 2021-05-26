#ifndef TORRENTPEEREAGER_NORELATIONS_H
#define TORRENTPEEREAGER_NORELATIONS_H

#include "orm/tiny/model.hpp"

using Orm::Tiny::Model;

class TorrentPeerEager_NoRelations final : public Model<TorrentPeerEager_NoRelations>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {"torrent_peers"};
};

#endif // TORRENTPEEREAGER_NORELATIONS_H