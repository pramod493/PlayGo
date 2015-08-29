#pragma once
#include "component.h"
#include <boost/graph/adjacency_list.hpp>	// how about adjacency_matrix?

using namespace boost;

namespace CDI
{
	class Assembly;
	class Component;
	class cdJoint;

	class AssemblyData
	{
		QHash<QUuid, Component*> components;
		QHash<QUuid, QGraphicsItem*> joints;	// TODO

		friend class Assembly;
	};

	class Assembly : public QGraphicsItemGroup
	{
	public:
		enum {Type = UserType + ASSEMBLYVIEW};

	protected:
		QUuid _id;

		AssemblyData* _data;

	public:
		Assembly(QGraphicsItem* parent = 0);

		virtual ~Assembly();

		int type() const { return Type; }

		QUuid id() const { return _id; }

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
