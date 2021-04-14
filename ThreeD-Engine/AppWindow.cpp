#include "AppWindow.h"
#include <Windows.h>
#include "Vector.h"
#include "InputSystem.h"

struct vertex
{
	vec3 position;
	vec2 texcoord;
};

__declspec(align(16))
struct constant
{
	mat4 m_transform;
	mat4 m_view;
	mat4 m_projection;
	unsigned int m_time=0;
};

AppWindow::AppWindow()
{
}

void AppWindow::update()
{
	constant cc;
	cc.m_time = (unsigned int)GetTickCount64();

	mat4 temp;

	cc.m_transform.setScale(vec3(m_scale_cube, m_scale_cube, m_scale_cube));

	temp.setIdentity();
	cc.m_transform *= temp;

	temp.setIdentity();
	cc.m_transform *= temp;

	temp.setIdentity();
	cc.m_transform *= temp;

	mat4 world_cam;
	world_cam.setIdentity();
	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	world_cam *= temp;
	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	world_cam *= temp;


	vec3 new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (m_forward*0.3f) + world_cam.getXDirection() * (m_rightward * 0.3f);

	temp.setTranslation(new_pos);
	world_cam *= temp;
	m_world_cam = world_cam;

	world_cam.inverse();



	cc.m_view = world_cam;
	/*cc.m_projection.setOrthoLH(
		(this->getClientWindowRect().right - this->getClientWindowRect().left) / 400.0f,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top) / 400.0f,
		-4.0f,
		4.0f
	);*/

	int width = this->getClientWindowRect().right - this->getClientWindowRect().left;
	int height = this->getClientWindowRect().bottom - this->getClientWindowRect().top;

	cc.m_projection.setPerspectiveFovLH(1.57f, (float)width / (float)height, 0.1f, 100.0f);


	m_cb->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);


	m_wood_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"Assets\\Textures\\brick.png");
	m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"Assets\\Meshes\\teapot.obj");



	RECT rc = this->getClientWindowRect();
	m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	m_world_cam.setTranslation(vec3(0, 0, -2));

	vec3 position_list[] =
	{
		{vec3(-0.5f,-0.5f,-0.5f)},
		{vec3(-0.5f, 0.5f,-0.5f)},
		{vec3(0.5f, 0.5f,-0.5f)},
		{vec3(0.5f, -0.5f,-0.5f)},

		{vec3(0.5f,-0.5f,0.5f)},
		{vec3(0.5f, 0.5f,0.5f)},
		{vec3(-0.5f, 0.5f,0.5f)},
		{vec3(-0.5f, -0.5f,0.5f)}
	};


	vec2 texcoord_list[] =
	{
		{vec2(0.0f, 0.0f)},
		{vec2(0.0f, 1.0f)},
		{vec2(1.0f, 0.0f)},
		{vec2(1.0f, 1.0f)},
	};

	vertex vertex_list[] =
	{
		{position_list[0], texcoord_list[1]},
		{position_list[1], texcoord_list[0]},
		{position_list[2], texcoord_list[2]},
		{position_list[3], texcoord_list[3]},

		{position_list[4], texcoord_list[1]},
		{position_list[5], texcoord_list[0]},
		{position_list[6], texcoord_list[2]},
		{position_list[7], texcoord_list[3]},

		{position_list[1], texcoord_list[1]},
		{position_list[6], texcoord_list[0]},
		{position_list[5], texcoord_list[2]},
		{position_list[2], texcoord_list[3]},

		{position_list[7], texcoord_list[1]},
		{position_list[0], texcoord_list[0]},
		{position_list[3], texcoord_list[2]},
		{position_list[4], texcoord_list[3]},

		{position_list[3], texcoord_list[1]},
		{position_list[2], texcoord_list[0]},
		{position_list[5], texcoord_list[2]},
		{position_list[4], texcoord_list[3]},

		{position_list[7], texcoord_list[1]},
		{position_list[6], texcoord_list[0]},
		{position_list[1], texcoord_list[2]},
		{position_list[0], texcoord_list[3]},
	};

	UINT size_list = ARRAYSIZE(vertex_list);

	unsigned int index_list[] =
	{
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
	};

	UINT size_index_list = ARRAYSIZE(index_list);
	m_ib = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(index_list, size_index_list);


	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	m_vb = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(vertex_list, (UINT)sizeof(vertex), size_list, shader_byte_code, (UINT)size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();


	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;

	m_cb = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor(m_swap_chain, 0, 0.3f, 0.4f, 1);

	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	update();

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(m_ps);


	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(m_ps, m_wood_tex);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(m_mesh->getVertexBuffer());
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(m_mesh->getIndexBuffer());

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(m_mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);
	m_swap_chain->present(false);

	m_old_delta = m_new_delta;
	m_new_delta = (float)::GetTickCount64();

	m_delta_time = m_old_delta?(m_new_delta - m_old_delta)/1000.0f:0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 0.5f;
}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 2.0f;
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'W') {
		m_forward = 1.0f;
		//m_rot_x += 0.707f * m_delta_time;
	}
	else if (key == 'S') {
		m_forward = -1.0f;
		//m_rot_x -= 0.707f * m_delta_time;
	}
	if (key == 'A') {
		m_rightward = -1.0f;
		//m_rot_y += 0.707f * m_delta_time;
	}
	else if (key == 'D') {
		m_rightward = 1.0f;
		//m_rot_y -= 0.707f * m_delta_time;
	}
}

void AppWindow::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	int width = this->getClientWindowRect().right - this->getClientWindowRect().left;
	int height = this->getClientWindowRect().bottom - this->getClientWindowRect().top;

	m_rot_x += (mouse_pos.y - (height / 2.0f)) * m_delta_time*0.3f;
	m_rot_y += (mouse_pos.x - (width / 2.0f)) * m_delta_time * 0.3f;

	InputSystem::get()->setCursorPosition(Point(width/2, height/2));
}
