#include "../ArduPilot/_AP_takeoff.h"

namespace kai
{

_AP_takeoff::_AP_takeoff()
{
	m_pAP = NULL;
	m_apMode = AP_COPTER_GUIDED;
	m_dAlt = INT16_MAX;
}

_AP_takeoff::~_AP_takeoff()
{
}

bool _AP_takeoff::init(void* pKiss)
{
	IF_F(!this->_MissionBase::init(pKiss));
	Kiss* pK = (Kiss*)pKiss;

	pK->v("apMode", &m_apMode);

	string iName;
	iName = "";
	pK->v("_AP_base", &iName);
	m_pAP = (_AP_base*) (pK->getInst(iName));
	IF_Fl(!m_pAP, iName + ": not found");

	return true;
}

bool _AP_takeoff::start(void)
{
	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		LOG(ERROR) << "Return code: "<< retCode;
		m_bThreadON = false;
		return false;
	}

	return true;
}

int _AP_takeoff::check(void)
{
	NULL__(m_pAP, -1);
	NULL__(m_pAP->m_pMav, -1);

	return this->_MissionBase::check();
}

void _AP_takeoff::update(void)
{
	while (m_bThreadON)
	{
		this->autoFPSfrom();

		this->_MissionBase::update();
		updateMission();

		this->autoFPSto();
	}
}

void _AP_takeoff::updateMission(void)
{
	IF_(check()<0);
	IF_(!bActive());
	if(m_apMode >= 0)
	{
		IF_(m_pAP->getApMode() != m_apMode);
	}

	Mission* pM = m_pMC->getMission();
	NULL_(pM);
	IF_(pM->type() != mission_takeoff);

	Takeoff* pTO = (Takeoff*)pM;
	m_dAlt = (float)m_pAP->m_vGlobalPos.w - pTO->m_alt;
	if(m_dAlt > -0.5)
	{
		pTO->complete();
		return;
	}

	m_pAP->m_pMav->clNavTakeoff(pTO->m_alt);
}

void _AP_takeoff::draw(void)
{
	IF_(check()<0);
	this->_MissionBase::draw();
	drawActive();

	addMsg("alt = "+f2str(m_pAP->m_vGlobalPos.w) + ", dAlt = " + f2str(m_dAlt));
}

}
