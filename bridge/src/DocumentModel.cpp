#include "bridge/DocumentModel.h"

namespace comicos {

DocumentModel::DocumentModel(QObject* parent) : QAbstractListModel(parent) {}
DocumentModel::~DocumentModel() = default;

void DocumentModel::setDocument(Document* document) {
    beginResetModel();
    m_document = document;
    endResetModel();
    emit canvasSizeChanged();
    emit activeLayerChanged();
}

int DocumentModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid() || !m_document) return 0;
    return m_document->layers().count();
}

QVariant DocumentModel::data(const QModelIndex& index, int role) const {
    if (!m_document || !index.isValid()) return {};

    // Reverse index: QML shows top layer first
    int layerIndex = m_document->layers().count() - 1 - index.row();
    const Layer* layer = m_document->layers().layerAt(layerIndex);
    if (!layer) return {};

    switch (role) {
    case IdRole:
        return QVariant::fromValue(static_cast<quint64>(layer->id()));
    case NameRole:
        return layer->name();
    case OpacityRole:
        return layer->opacity();
    case VisibleRole:
        return layer->isVisible();
    case LockedRole:
        return layer->isLocked();
    case BlendModeRole:
        return static_cast<int>(layer->blendMode());
    }

    return {};
}

bool DocumentModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!m_document || !index.isValid()) return false;

    int layerIndex = m_document->layers().count() - 1 - index.row();
    Layer* layer = m_document->layers().layerAt(layerIndex);
    if (!layer) return false;

    switch (role) {
    case NameRole:
        layer->setName(value.toString());
        break;
    case OpacityRole:
        layer->setOpacity(value.toFloat());
        break;
    case VisibleRole:
        layer->setVisible(value.toBool());
        break;
    case LockedRole:
        layer->setLocked(value.toBool());
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> DocumentModel::roleNames() const {
    return {
        {IdRole, "layerId"},
        {NameRole, "layerName"},
        {OpacityRole, "layerOpacity"},
        {VisibleRole, "layerVisible"},
        {LockedRole, "layerLocked"},
        {BlendModeRole, "layerBlendMode"},
    };
}

Qt::ItemFlags DocumentModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

void DocumentModel::addLayer() {
    if (!m_document) return;
    beginInsertRows({}, 0, 0);  // Top of list (reversed)
    m_document->layers().addLayer();
    endInsertRows();
    emit activeLayerChanged();
}

void DocumentModel::removeLayer(int index) {
    if (!m_document || m_document->layers().count() <= 1) return;

    int layerIndex = m_document->layers().count() - 1 - index;
    auto* layer = m_document->layers().layerAt(layerIndex);
    if (!layer) return;

    beginRemoveRows({}, index, index);
    m_document->layers().removeLayer(layer->id());
    endRemoveRows();
    emit activeLayerChanged();
}

void DocumentModel::duplicateLayer(int index) {
    if (!m_document) return;

    int layerIndex = m_document->layers().count() - 1 - index;
    auto* layer = m_document->layers().layerAt(layerIndex);
    if (!layer) return;

    beginInsertRows({}, index, index);
    m_document->layers().duplicateLayer(layer->id());
    endInsertRows();
}

void DocumentModel::moveLayer(int from, int to) {
    if (!m_document) return;

    int fromLayer = m_document->layers().count() - 1 - from;
    int toLayer = m_document->layers().count() - 1 - to;

    if (fromLayer == toLayer) return;

    beginMoveRows({}, from, from, {}, to > from ? to + 1 : to);
    m_document->layers().moveLayer(fromLayer, toLayer);
    endMoveRows();
}

void DocumentModel::setLayerName(int index, const QString& name) {
    setData(this->index(index), name, NameRole);
}

void DocumentModel::setLayerOpacity(int index, qreal opacity) {
    setData(this->index(index), opacity, OpacityRole);
}

void DocumentModel::setLayerVisible(int index, bool visible) {
    setData(this->index(index), visible, VisibleRole);
}

int DocumentModel::activeLayerIndex() const {
    if (!m_document) return -1;
    int idx = m_document->layers().indexOf(m_document->layers().activeLayerId());
    if (idx < 0) return -1;
    return m_document->layers().count() - 1 - idx;
}

void DocumentModel::setActiveLayerIndex(int index) {
    if (!m_document) return;
    int layerIndex = m_document->layers().count() - 1 - index;
    auto* layer = m_document->layers().layerAt(layerIndex);
    if (layer) {
        m_document->layers().setActiveLayerId(layer->id());
        emit activeLayerChanged();
    }
}

int DocumentModel::canvasWidth() const {
    return m_document ? m_document->canvasSize().width() : 0;
}

int DocumentModel::canvasHeight() const {
    return m_document ? m_document->canvasSize().height() : 0;
}

}  // namespace comicos
