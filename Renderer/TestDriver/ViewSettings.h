#ifndef VIEW_SETTINGS_H
#define VIEW_SETTINGS_H

namespace RasterRenderer
{
    namespace Testing
    {
        class ViewSettings
        {
        public:
            int WindowWidth, WindowHeight;
            float FovY, zNear, zFar;
            ViewSettings()
            {
                WindowWidth = 1024;
                WindowHeight = 768;
                FovY = 75.0f;
                zNear = 1.0f;
                zFar = 10000.0f;
            }
        };
    }
}

#endif