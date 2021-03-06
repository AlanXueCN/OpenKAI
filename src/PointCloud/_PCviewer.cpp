/*
 * _PCviewer.cpp
 *
 *  Created on: May 28, 2020
 *      Author: yankai
 */

#include "_PCviewer.h"

#ifdef USE_OPEN3D

namespace kai
{

_PCviewer::_PCviewer()
{
	m_vWinSize.init(1280, 720);
	m_pMcoordFrame = NULL;
	m_fov = 0.0;
}

_PCviewer::~_PCviewer()
{
}

bool _PCviewer::init(void *pKiss)
{
	IF_F(!this->_ThreadBase::init(pKiss));
	Kiss *pK = (Kiss*) pKiss;

	pK->v("vWinSize", &m_vWinSize);
	pK->v("fov", &m_fov);

	utility::SetVerbosityLevel(utility::VerbosityLevel::Error);

	//X:red, Y:green, Z:blue
	m_pMcoordFrame = open3d::geometry::TriangleMesh::CreateCoordinateFrame();

	return true;
}

bool _PCviewer::start(void)
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

int _PCviewer::check(void)
{
	return 0;
}

void _PCviewer::update(void)
{
	m_vis.CreateVisualizerWindow(*this->getName(), m_vWinSize.x, m_vWinSize.y);
	m_vis.GetRenderOption().background_color_ = Eigen::Vector3d::Zero();
	m_vis.GetViewControl().ChangeFieldOfView(m_fov);
	m_vis.AddGeometry(m_pMcoordFrame);

	while (m_bThreadON)
	{
		this->autoFPSfrom();

		render();

		this->autoFPSto();
	}

	m_vis.DestroyVisualizerWindow();
}

void _PCviewer::render(void)
{
	for(int i=0; i<m_vGeo.size(); i++)
	{
		PCVIEWER_GEO* pG =&m_vGeo[i];

		pG->addToVisualizer(&m_vis);
		pG->updateVisualizer(&m_vis);
	}

	if (m_vis.HasGeometry())
	{
		m_vis.PollEvents();
		m_vis.UpdateRender();
	}
}

int _PCviewer::addGeometry(void)
{
	PCVIEWER_GEO g;
	g.init();

	m_vGeo.push_back(g);
	return m_vGeo.size()-1;
}

void _PCviewer::updateGeometry(int i, PointCloud* pPC)
{
	IF_(i >= m_vGeo.size());
	IF_(i < 0);

	m_vGeo[i].updateGeometry(pPC);
}

void _PCviewer::draw(void)
{
	this->_ThreadBase::draw();
}

}
#endif
