#include "material.h"

namespace CDI
{
	Material::Material()
	{
		_friction = 0.1f;
		_restitution = 0.1f;
		_density = 1.0f;
		_materialName = QString("No Material");
	}

	Material::Material(float friction, float restitution, float density, QString name)
	{
		_friction		= friction;
		_restitution	= restitution;
		_density		= density;
		_materialName	= name;
	}

	Material::Material(const Material &mat)
	{
		_friction = mat._friction;
		_restitution = mat._restitution;
		_density = mat._density;
		_materialName = mat._materialName;
	}

	float Material::friction() const
	{
		return _friction;
	}

	void Material::setFriction(float friction)
	{
		_friction = friction;
		emit materialChanged(this);
	}

	float Material::restitution() const
	{
		return _restitution;
	}

	void Material::setRestitution(float restitution)
	{
		_restitution = restitution;
		emit materialChanged(this);
	}

	float Material::density() const
	{
		return _density;
	}

	void Material::setDensity(float density)
	{
		_density = density;
		emit materialChanged(this);
	}

	QString Material::materialName() const
	{
		return _materialName;
	}

	void Material::setMaterialName(const QString &materialName)
	{
		_materialName = materialName;
		emit materialChanged(this);
	}

	/*------------------------------------*/
	MaterialDb::MaterialDb()
	{
//		pInternalList.in(new Material(0.5, 0.5, 7750, "steel"));
//		pInternalList(new Material(0.5, 0.603, 1000, "wood"));
	}
}

