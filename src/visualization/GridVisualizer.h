#ifndef SWARMING_PROJECT_GRIDVISUALIZER_H
#define SWARMING_PROJECT_GRIDVISUALIZER_H

#include <vtkSmartPointer.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include "definitions/types.h"
#include "definitions/graphical_constants.h"
#include "data_structures/Grid.h"
#include "data_structures/Boid.h"
#include "visualization/vtkTimerCallback.h"

using types::Position;

template <typename Distribution, std::size_t Dimension>
class GridVisualizer {

    static_assert(Dimension == 2 || Dimension == 3, "The Dimension of the Visualizer can only be 2 or 3.");

    using TimerCallback = vtkTimerCallback<Distribution, Dimension>;

public:

    explicit GridVisualizer(Grid<Distribution, Dimension> & grid)
            : m_grid(grid),
              m_renderer(vtkSmartPointer<vtkRenderer>::New()),
              m_render_window(vtkSmartPointer<vtkRenderWindow>::New()),
              m_render_window_interactor(vtkSmartPointer<vtkRenderWindowInteractor>::New())
    {
        initialize_boids();
        m_render_window->AddRenderer(m_renderer);
        m_render_window_interactor->SetRenderWindow(m_render_window);

        // Now register the time event for the updates.
        m_render_window_interactor->Initialize();

        // Sign up to receive TimerEvent
        vtkSmartPointer<TimerCallback> callback = TimerCallback::New(m_grid, m_renderer);
        m_render_window_interactor->AddObserver(vtkCommand::TimerEvent, callback);

        // Create the TimerEvent
        m_render_window_interactor->CreateRepeatingTimer(gconst::UPDATE_DELAY_MS);
    }


    void start() {
        // Launch the visualization
        m_render_window_interactor->Start();
    }

    void initialize_boids() {
        std::size_t i{0};
        for(const auto & boid : m_grid.m_boids) {
            create_boid_for_rendering(boid);
            ++i;
        }
        std::cout << "Created " << i << " boids." << std::endl;
    }

private:

    void create_boid_for_rendering(const Boid<Distribution, Dimension> &boid)
    {
        double buffer[gconst::VTK_COORDINATES_NUMBER];

        // Compute the center of the circle
        for(std::size_t i{0}; i < Dimension; ++i) {
            buffer[i] = static_cast<double>(boid.m_position[i]);
        }

        // Create a circle
        vtkSmartPointer<vtkRegularPolygonSource> circle =
                vtkSmartPointer<vtkRegularPolygonSource>::New();
        //circle->GeneratePolygonOff();
        circle->SetNumberOfSides(gconst::BOID_NUMBER_OF_SIDES);
        circle->SetRadius(gconst::BOID_RADIUS);

        // Create the mapper and the actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(circle->GetOutputPort());;

        vtkSmartPointer<vtkActor> actor =
                vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->SetPosition(buffer);

        // Add the created actor to the renderer
        m_renderer->AddActor(actor);
    }


    Grid<Distribution, Dimension> & m_grid;

    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderWindow> m_render_window;
    vtkSmartPointer<vtkRenderWindowInteractor> m_render_window_interactor;
};


#endif //SWARMING_PROJECT_GRIDVISUALIZER_H
