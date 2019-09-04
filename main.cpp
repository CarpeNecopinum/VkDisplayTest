#include <lava/gpuselection/NthOfTypeStrategy.hh>
#include <lava/objects/Device.hh>
#include <lava/objects/Instance.hh>
#include <lava/objects/Image.hh>
#include <lava-extras/display/DisplayOutput.hh>
#include <lava-extras/display/DisplayWindow.hh>

/// Simple Testing setup: use the first screen on the first device
//const auto n_devices = 1;
//std::vector<std::pair<int, int>> display_idcs = {{0, 0}};

/// Complicated setup: 4 devices, one screen on each
//const auto n_devices = 4;
//std::vector<std::pair<int, int>> display_idcs = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};

/// Simplest failing setup: 2 devices, 5 screens
const auto n_devices = 2;
std::vector<std::pair<int, int>> display_idcs = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}};

int main()
{
    auto display = lava::display::DisplayOutput::create();
    auto instance = lava::Instance::create({display});

    std::vector<lava::SharedDevice> devices;
    for (int i = 0; i < n_devices; i++) {
        auto device = instance->createDevice(
            {lava::QueueRequest::graphics("graphics")},
            lava::NthOfTypeStrategy(vk::PhysicalDeviceType::eDiscreteGpu, i));

        devices.push_back(device);
    }

    std::vector<lava::display::SharedDisplayWindow> windows;
    for (auto pair : display_idcs) {
        windows.push_back(display->openWindow(devices[pair.first], pair.second));
        windows.back()->buildSwapchainWith([](auto const&) {}); // Don't worry about framebuffers
    }

    for (int round = 0; round < 10; round++) {
        for (int i = 0; i < windows.size(); i++) {
            auto const& window = windows[i];
            auto const& device = window->device();
            {
                auto frame = window->startFrame();
                auto cmd = window->device()->graphicsQueue().beginCommandBuffer();
                frame.image()->changeLayout(vk::ImageLayout::ePresentSrcKHR, cmd);

                // Connect semaphores
                cmd.wait(frame.imageReady());
                cmd.signal(frame.renderingComplete());
            }
            //Wait till device is done, so we know earlier when it breaks
            device->graphicsQueue().catchUp(0);

            std::cout << "Round: " << round << " Display: " << i << std::endl;
        }
    }

    std::cout << "Everything worked!" << std::endl;
}
