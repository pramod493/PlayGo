#pragma once
#include "commonfunctions.h"
#include "assembly.h"

/**
 \mainpage PlayGo core component documentation
 \section Introducion to datastructure
 All objetcs are derived from AbstractModelItem object
 \section Conact Info
Author: Pramod Kumar
email: pramod@purdue.edu
email: pramod493@gmail.com
 */
namespace CDI
{
	/**
	 * @brief
	 * The root object which contains all the components.
	 * Mostly used for loading/saving files and maintaining ID unique
	 */
	class PlayGo
	{
	public:
		PlayGo();
		~PlayGo();
	};
}
