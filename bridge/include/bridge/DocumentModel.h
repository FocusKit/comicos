#pragma once

#include "core/Document.h"
#include "core/Types.h"
#include <QAbstractListModel>
#include <QQmlEngine>

namespace comicos {

/// QAbstractListModel exposing the layer stack to QML.
/// Provides a standard Qt model interface for the layer panel.
class DocumentModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int activeLayerIndex READ activeLayerIndex WRITE setActiveLayerIndex NOTIFY activeLayerChanged)
    Q_PROPERTY(int canvasWidth READ canvasWidth NOTIFY canvasSizeChanged)
    Q_PROPERTY(int canvasHeight READ canvasHeight NOTIFY canvasSizeChanged)

public:
    enum LayerRole {
        IdRole = Qt::UserRole + 1,
        NameRole,
        OpacityRole,
        VisibleRole,
        LockedRole,
        BlendModeRole,
    };

    explicit DocumentModel(QObject* parent = nullptr);
    ~DocumentModel() override;

    void setDocument(Document* document);

    // --- QAbstractListModel ---
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // --- Layer Operations (invocable from QML) ---
    Q_INVOKABLE void addLayer();
    Q_INVOKABLE void removeLayer(int index);
    Q_INVOKABLE void duplicateLayer(int index);
    Q_INVOKABLE void moveLayer(int from, int to);
    Q_INVOKABLE void setLayerName(int index, const QString& name);
    Q_INVOKABLE void setLayerOpacity(int index, qreal opacity);
    Q_INVOKABLE void setLayerVisible(int index, bool visible);

    // --- Properties ---
    int activeLayerIndex() const;
    void setActiveLayerIndex(int index);
    int canvasWidth() const;
    int canvasHeight() const;

signals:
    void activeLayerChanged();
    void canvasSizeChanged();

private:
    Document* m_document = nullptr;
};

}  // namespace comicos
