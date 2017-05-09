#include "Renderer.hpp"
#include "Window.hpp"
#include <cstring>
#include <iostream>
#include "Particle.png.hpp"

int main(int argc, char* argv[]) {
    Window window;
    
    bool measure = false;
    if (argc > 1 && strcmp(argv[1], "measure") == 0) {
        std::cout << "Measuring..." << std::endl;
        measure = true;
    }
    
    Renderer* renderer = new Renderer(window);
    renderer->setTexture(PARTICLE_PNG, PARTICLE_PNG_LENGTH);
    
    double totalTime = 0.0;
    unsigned int frame = 0;
    double maxTime = 0.0;
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        
        // Update and render.
        renderer->update(measure ? 1.0/60.0 : deltaTime);
        renderer->render();
        
        glfwSwapBuffers(window.getWindow());
        
        // Measurements.
        totalTime += deltaTime;
        if (deltaTime > maxTime)
            maxTime = deltaTime;
        
        if (frame++ >= 3600 && measure)
            break;
        
        // Poll events.
        glfwPollEvents();
    }
    
    delete renderer;
    
    // Output results.
    std::cout << "Average frame time: " << totalTime / frame * 1000.0 << " ms" << std::endl;
    std::cout << "Max frame time: " << maxTime * 1000.0 << " ms" << std::endl;
    
    return 0;
}
