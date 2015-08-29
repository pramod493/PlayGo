#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <QObject>
namespace CDI
{
	class Material : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(float friction READ friction WRITE setFriction)
		Q_PROPERTY(float restitution READ restitution WRITE setRestitution)
		Q_PROPERTY(float density READ density WRITE setDensity)
		Q_PROPERTY(QString name READ materialName WRITE setMaterialName)

	protected:
		float _friction;
		float _restitution;
		float _density;
		QString _materialName;
	public :
		Material();

		Material(float friction, float restitution, float density, QString name);

		Material(const Material& mat);

		float friction() const;
		void setFriction(float friction);

		float restitution() const;
		void setRestitution(float restitution);

		float density() const;
		void setDensity(float density);

		QString materialName() const;
		void setMaterialName(const QString &materialName);

	signals:
		void materialChanged(Material* mat);
	};

	class MaterialDb : public QObject
	{
		Q_OBJECT
		//QHash<QString, Material*> pInternalList;

	public :
		MaterialDb();
	};
}
#endif // __MATERIAL_H__
