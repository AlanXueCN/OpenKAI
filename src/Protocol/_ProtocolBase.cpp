#include "_ProtocolBase.h"

namespace kai
{
    
_ProtocolBase::_ProtocolBase()
{
	m_pIO = NULL;
	m_pBuf = NULL;
	m_nBuf = 256;
	m_nCMDrecv = 0;

	m_pfCallback = NULL;
	m_pfInst = NULL;
}

_ProtocolBase::~_ProtocolBase()
{
}

bool _ProtocolBase::init(void* pKiss)
{
	IF_F(!this->_ThreadBase::init(pKiss));
	Kiss* pK = (Kiss*)pKiss;

	pK->v("nBuf", &m_nBuf);
	m_pBuf = new uint8_t[m_nBuf];
	m_recvMsg.init(m_nBuf);

	string n;
	n = "";
	F_ERROR_F(pK->v("_IOBase", &n));
	m_pIO = (_IOBase*) (pK->getInst(n));
	NULL_Fl(m_pIO, n + ": not found");

	return true;
}

bool _ProtocolBase::start(void)
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

int _ProtocolBase::check(void)
{
	NULL__(m_pIO,-1);
	IF__(!m_pIO->isOpen(),-1);
	NULL_F(m_pBuf);

	return 0;
}

void _ProtocolBase::update(void)
{
	while (m_bThreadON)
	{
		if(!m_pIO)
		{
			this->sleepTime(USEC_1SEC);
			continue;
		}

		if(!m_pIO->isOpen())
		{
			this->sleepTime(USEC_1SEC);
			continue;
		}

		this->autoFPSfrom();

		while(readCMD())
		{
			handleCMD();
			m_nCMDrecv++;
		}

		this->autoFPSto();
	}
}

bool _ProtocolBase::readCMD(void)
{
	uint8_t	b;
	int		nB;

	while ((nB = m_pIO->read(&b,1)) > 0)
	{
		if (m_recvMsg.m_cmd != 0)
		{
			m_recvMsg.m_pB[m_recvMsg.m_iB] = b;
			m_recvMsg.m_iB++;

			if (m_recvMsg.m_iB == 3)
			{
				m_recvMsg.m_nPayload = m_recvMsg.m_pB[2];
			}
			
			IF_T(m_recvMsg.m_iB == m_recvMsg.m_nPayload + PB_N_HDR );
		}
		else if (b == PB_BEGIN )
		{
			m_recvMsg.m_cmd = b;
			m_recvMsg.m_pB[0] = b;
			m_recvMsg.m_iB = 1;
			m_recvMsg.m_nPayload = 0;
		}
	}

	return false;
}

void _ProtocolBase::handleCMD(void)
{
	if(m_pfCallback)
	{
		m_pfCallback(m_recvMsg.m_pB, m_pfInst);
	}

	m_recvMsg.reset();
}

void _ProtocolBase::setCallback(CallbackProtocol cb, void* pInst)
{
	NULL_(cb);
	NULL_(pInst);

	m_pfCallback = cb;
	m_pfInst = pInst;
}

void _ProtocolBase::draw(void)
{
	this->_ThreadBase::draw();

	if (!m_pIO->isOpen())
	{
		addMsg("Not Connected",1);
		return;
	}

	addMsg("nCMD = " + i2str(m_nCMDrecv),1);
}

}
