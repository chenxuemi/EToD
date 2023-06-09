#pragma once

#include "ETOD/Core/Layer.h"

#include "ETOD/Events/ApplicationEvent.h"
#include "ETOD/Events/KeyEvent.h"
#include "ETOD/Events/MouseEvent.h"

namespace ETOD {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColors();

		uint32_t GetActiveWidgetID() const;

	public:
		// Language
		int language = 0;

	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}