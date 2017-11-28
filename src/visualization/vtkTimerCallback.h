#ifndef SWARMING_PROJECT_VTKTIMERCALLBACK_H
#define SWARMING_PROJECT_VTKTIMERCALLBACK_H

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>

#include <vector>
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
     * @param grid         represent the space we want to simulate and visualize.
     * @param renderer     internal VTK structure used to render the image.
     * @param boids_actors array of the actors that represent the boids.
     * @return             a pointer over the newly-created vtkTimerCallback instance.
     */
    static vtkTimerCallback *New(Grid<Distribution, Dimension> &grid,
                                 vtkSmartPointer<vtkRenderer> renderer,
                                 std::vector<vtkSmartPointer<vtkActor> > &boids_actors) {
        return new vtkTimerCallback<Distribution, Dimension>(grid, renderer, boids_actors);
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
     * @param grid         represent the space we want to simulate and visualize.
     * @param renderer     internal VTK structure used to render the image.
     * @param boids_actors array of the actors that represent the boids.
     */
    explicit vtkTimerCallback(Grid<Distribution, Dimension> &grid,
                              vtkSmartPointer<vtkRenderer> renderer,
                              std::vector<vtkSmartPointer<vtkActor> > &boids_actors)
            : m_grid(grid),
              m_renderer(renderer),
              m_boids_actors(boids_actors) {}

    /**
     * Update all the boids on the visualization.
     */
    void update_boids() {
        m_grid.update_all_boids();

        for (std::size_t i{0}; i < m_grid.m_boids.size(); ++i) {
            double boid_position[gconst::VTK_COORDINATES_NUMBER];
            // Cast the boid position as double to give it to VTK.
            for (std::size_t j{0}; j < Dimension; ++j) {
                boid_position[j] = static_cast<double>(m_grid.m_boids[i].m_position[j]);
            }
            m_boids_actors[i]->SetPosition(boid_position);
        }
    }

    Grid<Distribution, Dimension> & m_grid;
    vtkSmartPointer<vtkRenderer>    m_renderer;
    std::vector<vtkSmartPointer<vtkActor> > &m_boids_actors;

};


#endif //SWARMING_PROJECT_VTKTIMERCALLBACK_H
