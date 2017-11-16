#ifndef SWARMING_PROJECT_VTKTIMERCALLBACK_H
#define SWARMING_PROJECT_VTKTIMERCALLBACK_H

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>

#include <iostream>

#include "data_structures/Grid.h"

template <typename Distribution, std::size_t Dimension>
class vtkTimerCallback : public vtkCommand {

public:

    explicit vtkTimerCallback(Grid<Distribution, Dimension> & grid,
                              vtkSmartPointer<vtkRenderer> renderer)
            : m_grid(grid),
              m_renderer(renderer)
    { }

    static vtkTimerCallback *New(Grid<Distribution, Dimension> & grid, vtkSmartPointer<vtkRenderer> renderer) {
        return new vtkTimerCallback<Distribution, Dimension>(grid, renderer);
    }

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

    void update_boids() {
        double buffer[gconst::VTK_COORDINATES_NUMBER];

        m_grid.shuffle();

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
