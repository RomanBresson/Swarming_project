#ifndef SWARMING_PROJECT_VTKTIMERCALLBACK_H
#define SWARMING_PROJECT_VTKTIMERCALLBACK_H

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>

#include <iostream>

#include "data_structures/Grid.h"

/**
 * Contains code that will be called in the VTK's event-loop.
 * @tparam Distribution probability distribution used to create the boids.
 * @tparam Dimension    dimension of the simulation.
 */
template <typename Distribution, std::size_t Dimension>
class vtkTimerCallback : public vtkCommand {

public:

    /**
     * Part of the VTK's interface for vtkCommand.
     *
     * Create a new instance of vtkTimerCallback.
     *
     * @param grid     represent the space we want to simulate and visualize.
     * @param renderer internal VTK structure used to render the image.
     * @return         a pointer over the newly-created vtkTimerCallback instance.
     */
    static vtkTimerCallback *New(Grid<Distribution, Dimension> & grid, vtkSmartPointer<vtkRenderer> renderer) {
        return new vtkTimerCallback<Distribution, Dimension>(grid, renderer);
    }

    /**
     * Part of the VTK's interface for vtkCommand.
     * @param caller  the instance that called this method.
     * @param eventId the event that trigered this call.
     */
    void Execute(vtkObject *caller, unsigned long eventId,
                 void * vtkNotUsed(callData)) override {
        if(eventId == vtkCommand::TimerEvent) {
            std::cout << "Updating..." << std::flush;
            update_boids();
            vtkRenderWindowInteractor *render_window_interactor{vtkRenderWindowInteractor::SafeDownCast(caller)};
            render_window_interactor->GetRenderWindow()->Render();
            std::cout << "\tUpdate done!" << std::endl;
        }
    }

private:

    /**
     * Construct an instance of vtkTimerCallback
     * @param grid     represent the space we want to simulate and visualize.
     * @param renderer internal VTK structure used to render the image.
     */
    explicit vtkTimerCallback(Grid<Distribution, Dimension> &grid,
                              vtkSmartPointer<vtkRenderer> renderer)
            : m_grid(grid),
              m_renderer(renderer) {}

    /**
     * Update all the boids on the visualization.
     */
    void update_boids() {
        double buffer[gconst::VTK_COORDINATES_NUMBER];

        m_grid.update_all_boids();

        auto actors = m_renderer->GetActors();
        actors->InitTraversal();

        for(const auto & boid : m_grid.m_boids) {
            // Cast the boid position as double to give it to VTK.
            for(std::size_t i{0}; i < Dimension; ++i) {
                buffer[i] = static_cast<double>(boid.m_position[i]);
            }
            actors->GetNextActor()->SetPosition(buffer);
        }
    }

    Grid<Distribution, Dimension> & m_grid;
    vtkSmartPointer<vtkRenderer>    m_renderer;

};


#endif //SWARMING_PROJECT_VTKTIMERCALLBACK_H
