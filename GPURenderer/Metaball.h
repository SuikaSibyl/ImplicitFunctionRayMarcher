#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include<algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace SuikaGL
{
	class Metaball
	{
	public:
        bool cmp(glm::vec4 tmp1, glm::vec4 tmp2)
        {
            return tmp1.w > tmp2.w;
        }

		Metaball(std::string path, float scale = 1)
		{
            std::ifstream ifile;
            ifile.open(path, std::ios::in);
            if (!ifile)
            {
                std::cerr << "Open File Fail." << std::endl;
                exit(1);
            }

            ifile >> num;
            std::cout << "Metaball Num: " << num << std::endl;
            pointer = new glm::vec4[num];
            for (int t = 0; t < num; ++t)
            {
                glm::vec4 value(0);
                ifile >> value.x >> value.y >> value.z >> value.w;
                std::cout << "Metaball Para: "<< value.x << value.y << value.z << value.w << std::endl;
                *(pointer + t) = value * scale;
            }

            for (int j = 0; j < num; j++)
                for (int k = 0; k < num - j - 1; k++)
                {
                    if (pointer[k].w > pointer[k + 1].w)
                    {
                        glm::vec4 temp = pointer[k];
                        pointer[k] = pointer[k + 1];
                        pointer[k + 1] = temp;
                    }
                }

            ifile.close();
		}

        Metaball(glm::vec4* in_pointer, int in_num)
        {
            pointer = in_pointer;
            num = in_num;
        }

        ~Metaball()
        {
            //delete[] pointer;
        }
        
        Metaball blend(Metaball& meta2, float alpha)
        {
            Metaball& meta1 = *this;

            int min = (meta1.num > meta2.num) ? meta2.num : meta1.num;

            glm::vec4* pointer = new glm::vec4[min];
            for (int i = 0; i < min; i++)
            {
                glm::vec4 tmp1 = meta1.pointer[i];
                glm::vec4 tmp2 = meta2.pointer[i];
                pointer[i] = (1 - alpha) * tmp1 + alpha * tmp2;
            }

            return Metaball(pointer, min);
        }

        glm::vec4* pointer;
        int num;

	private:
	};
}