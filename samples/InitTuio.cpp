
/****************************************************************
 * Initialize TUIO and attach to the sketch view
 * **************************************************************/
void InitTuio() 
{
#ifdef Q_OS_LINUX
#ifdef ENABLE_TUIO
	bool tuio_enabled = false;
	if (tuio_enabled)	// Set to true if TUIO is enabled
	{
		// default port is 3333. Use multiple ports for multi-screen
		QTuio* tuio = new QTuio(window->sketchView);
		tuio->run();
	}
#endif //ENABLE_TUIO
#endif //Q_OS_LINUX
}