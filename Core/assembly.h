#pragma once
#include <memory>
#include <QUuid>
#include <QDataStream>
#include <QGraphicsItemGroup>
#include "commonfunctions.h"
#include "cdi2qtwrapper.h"

namespace CDI
{
	class Component;
	class AssemblyPrivateData;
	class Assembly : public QGraphicsItemGroup
	{
	public:
		enum {Type = UserType + ASSEMBLYVIEW};

	protected:
		QUuid _id;

		std::unique_ptr<AssemblyPrivateData> _data;

	public:
		Assembly(QGraphicsItem* parent = 0);

		virtual ~Assembly();

		int type() const;

		QUuid id() const;

		virtual void addComponent(Component* component);

		virtual void removeComponent(Component* component);

		virtual void removeComponent(QUuid componentId);

		virtual bool containsComponent(QUuid componentId);

		virtual QList<Component*> components();

		virtual bool mergeAssembly(Assembly* a);

		virtual Component* getComponentById(QUuid id);

		virtual void addJoint(Component* c1, Component* c2);

		virtual QDataStream& serialize(QDataStream& stream) const;

		virtual QDataStream& deserialize(QDataStream& stream);

	protected:
		virtual void removeComponentConnections(Component* component) {Q_UNUSED(component)}

	};
}
