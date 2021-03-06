/*
 * _PCio.cpp
 *
 *  Created on: Oct 8, 2020
 *      Author: yankai
 */

#include "_PCsend.h"

#ifdef USE_OPEN3D

namespace kai
{

_PCsend::_PCsend()
{
	m_pIO = NULL;
	m_pB = NULL;
	m_nB = 256;
    m_tInt = 100000;
}

_PCsend::~_PCsend()
{
}

bool _PCsend::init(void *pKiss)
{
	IF_F(!_PCbase::init(pKiss));
	Kiss *pK = (Kiss*) pKiss;

	pK->v("tInt", &m_tInt);
	pK->v("nB", &m_nB);
	m_pB = new uint8_t[m_nB];
    NULL_F(m_pB);

	string n;
	n = "";
	F_ERROR_F(pK->v("_IOBase", &n));
	m_pIO = (_IOBase*) (pK->getInst(n));
	NULL_Fl(m_pIO, "_IOBase not found");

	return true;
}

bool _PCsend::start(void)
{
	IF_F(!this->_ThreadBase::start());

	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		m_bThreadON = false;
		return false;
	}

	return true;
}

int _PCsend::check(void)
{
	NULL__(m_pPCB, -1);
	NULL__(m_pIO, -1);
	IF__(!m_pIO->isOpen(),-1);

	return 0;
}

void _PCsend::update(void)
{
	while (m_bThreadON)
	{
		this->autoFPSfrom();

        sendPC();

		this->autoFPSto();
	}
}

void _PCsend::sendPC(void)
{
	IF_(check()<0);

	PointCloud pcOut;
    m_pPCB->getPC(&pcOut);
	int nP = pcOut.points_.size();
    IF_(nP <= 0);

    const double PC_SCALE = 1000;
    const int PC_DB = 2;
    m_pB[0] = PB_BEGIN;
    m_pB[1] = PC_STREAM;
    int iB = PC_N_HDR;
    
	for (int i = 0; i < nP; i++)
	{
        IF_CONT(pcOut.points_.empty());
        IF_CONT(pcOut.colors_.empty());
        
        Eigen::Vector3d vP = pcOut.points_[i];
        Eigen::Vector3d vC = pcOut.colors_[i];
//        Eigen::Vector3d vN = pcOut.normals_[i];
        
        pack_int16(&m_pB[iB], (int16_t)(vP.x() * PC_SCALE), false);
        iB += PC_DB;
        pack_int16(&m_pB[iB], (int16_t)(vP.y() * PC_SCALE), false);
        iB += PC_DB;
        pack_int16(&m_pB[iB], (int16_t)(vP.z() * PC_SCALE), false);
        iB += PC_DB;

        pack_int16(&m_pB[iB], (int16_t)(vC.x() * PC_SCALE), false);
        iB += PC_DB;
        pack_int16(&m_pB[iB], (int16_t)(vC.y() * PC_SCALE), false);
        iB += PC_DB;
        pack_int16(&m_pB[iB], (int16_t)(vC.z() * PC_SCALE), false);
        iB += PC_DB;

        if(iB + PC_DB * 6 > m_nB)
        {
            pack_int16(&m_pB[2], (int16_t)(iB - PC_N_HDR), false);
            while(!m_pIO->write(m_pB, iB))
                this->sleepTime(m_tInt);

            iB = PC_N_HDR;
        }
    }
    
    if(iB > PC_N_HDR)
    {
        pack_int16(&m_pB[2], (int16_t)(iB - PC_N_HDR), false);
        while(!m_pIO->write(m_pB, iB))
            this->sleepTime(m_tInt);
    }

    this->sleepTime(m_tInt);

    //frame sync
    m_pB[0] = PB_BEGIN;
    m_pB[1] = PC_FRAME_END;
    m_pB[2] = 0;
    m_pB[3] = 0;
    while(!m_pIO->write(m_pB, PC_N_HDR))
        this->sleepTime(m_tInt);

}

}
#endif
