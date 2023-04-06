#pragma once
#include "../New-Project/vendor/imgui/imgui.h"

namespace MultiText
{
	std::string endl = "/n";

	struct MultiPrecision
	{
		MultiPrecision(int precision) : value(precision) {};
		int value;
	};

	struct Image
	{
		Image(IDirect3DTexture9* image, ImVec2 size) : image(image), size(size) {};
		IDirect3DTexture9* image;
		ImVec2 size;
	};

	inline MultiPrecision setPrecision(int precision)
	{
		return MultiPrecision(precision);
	}

	class ImMultiText
	{
	public:

		~ImMultiText()
		{
			if (this->colorCount >= 1)
				ImGui::PopStyleColor(this->colorCount);
			if (this->fontCount >= 1)
				ImGui::PopFont();
		}

		inline ImMultiText& operator<<(const ImColor& color)
		{
			// we do this to prevent our colorcount ever going higher then 1, easier to keep track of
			if (this->colorCount >= 1)
			{
				ImGui::PopStyleColor(this->colorCount);
				this->colorCount = 0;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, color.Value);
			this->colorCount++;
			return *this;
		}

		inline ImMultiText& operator<<(const ImVec4& color)
		{
			// we do this to prevent our colorcount ever going higher then 1, easier to keep track of
			if (this->colorCount >= 1)
			{
				ImGui::PopStyleColor(this->colorCount);
				this->colorCount = 0;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, color);
			this->colorCount++;
			return *this;
		}

		inline ImMultiText& operator<<(ImFont* font)
		{
			// we do this to prevent our fontcount ever going higher then 1, easier to keep track of
			if (this->fontCount >= 1)
			{
				ImGui::PopFont();
				this->fontCount = 0;
			}
			ImGui::PushFont(font);
			this->fontCount++;
			return *this;
		}


		inline ImMultiText& operator<<(MultiPrecision precision)
		{
			this->decimalPrecision = precision.value;
			return *this;
		}

		inline ImMultiText& operator<<(const std::string& text)
		{
			if (text == endl)
				this->newLine();
			else
			{
				this->sameLine();
				ImGui::Text(text.c_str());
			}
			return *this;
		}

		inline ImMultiText& operator<<(const char* text)
		{
			if (text == endl)
				this->newLine();
			else
			{
				this->sameLine();
				ImGui::Text(text);
			}
			return *this;
		}

		inline ImMultiText& operator<<(Image image)
		{
			this->sameLine();
			ImGui::Image(image.image, image.size);
			return *this;
		}


		template<class T>

		inline ImMultiText& operator<<(const T& value)
		{
			this->sameLine();
			if (this->decimalPrecision != -1 && (typeid(T) == typeid(float) || typeid(T) == typeid(double)))
			{
				std::ostringstream ss;
				ss << std::fixed << std::setprecision(this->decimalPrecision) << value;
				ImGui::Text(ss.str().c_str());
			}
			else
				ImGui::Text(std::to_string(value).c_str());

			return *this;
		}

	private:
		int colorCount = 0;
		int fontCount = 0;
		int decimalPrecision = -1;

		inline void sameLine()
		{
			ImGui::SameLine();
		}

		inline void newLine()
		{
			ImGui::NewLine();
		}
	};


	// we do this to auto call the destructor after all the usages of the operator overload
	inline ImMultiText text()
	{
		return ImMultiText();
	}

}