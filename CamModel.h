#include "LoadModel.h"

namespace CDI
{
	class CamModel : public LoadModel
	{
		Q_OBJECT
	public:
		CamModel(PlayGoController *controller)
		
		void createComponents();

	public slots:
		void onSimulationStepComplete();

		void onSimulationStepStart();
	};
}