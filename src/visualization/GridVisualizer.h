#ifndef SWARMING_PROJECT_GRIDVISUALIZER_H
#define SWARMING_PROJECT_GRIDVISUALIZER_H

#include <vtkSmartPointer.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>

#include "definitions/types.h"
#include "definitions/graphical_constants.h"
#include "data_structures/Grid.h"
#include "data_structures/Boid.h"
#include "visualization/vtkTimerCallback.h"

using types::Position;

/**
 * Visualizer for the Grid class.
 *
 * @tparam Distribution probability distribution used for the boids in the grid we want to visualize.
 * @tparam Dimension    dimension of the space represented by the grid we want to visualize.
 */
template <typename Distribution, std::size_t Dimension>
class GridVisualizer {

    static_assert(Dimension == 2 || Dimension == 3, "The Dimension of the Visualizer can only be 2 or 3.");

    using TimerCallback = vtkTimerCallback<Distribution, Dimension>;

public:

    /**
     * Construct an instance of Visualizer.
     *
     * The grid must be given as a non-const reference because we will update it directly
     * in the event-loop of VTK.
     *
     * @param grid a non-constant reference on the grid we want to visualize.
     */
    explicit GridVisualizer(Grid<Distribution, Dimension> & grid)
            : m_grid(grid),
              m_renderer(vtkSmartPointer<vtkRenderer>::New()),
              m_render_window(vtkSmartPointer<vtkRenderWindow>::New()),
              m_render_window_interactor(vtkSmartPointer<vtkRenderWindowInteractor>::New()) {
        initialize_boids();
        m_renderer->SetBackground(gconst::BACKGROUND_COLOR);
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

    /**
     * Launch the visualization.
     *
     * Calls vtkRenderWindowInteractor::Start and start the event-loop of VTK.
     */
    void start() {
        // Launch the visualization
        m_render_window_interactor->Start();
    }

    /**
     * Initialize the data structure for plotting the boids.
     */
    void initialize_boids() {
        std::size_t i{0};
        for(const auto & boid : m_grid.m_boids) {
            create_boid_for_rendering(boid);
            ++i;
        }
        std::cout << "Created " << i << " boids." << std::endl;
    }

private:

    /**
     * Create all the data structures needed to visualize one boid.
     *
     * For the moment, the visualization of a boid is only a circle. We can improve
     * this by representing a boid as a triangle and take into account the current
     * orientation of the boid.
     *
     * @param boid boid we want to add to the visualization.
     */
    void create_boid_for_rendering(const Boid<Distribution, Dimension> &boid) {
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
        circle->SetRadius(gconst::BOID_RADIUS_COEFFICIENT * m_grid.m_top_right[0]);

        // Create the mapper and the actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(circle->GetOutputPort());;

        vtkSmartPointer<vtkActor> actor =
                vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->SetPosition(buffer);
        actor->GetProperty()->SetColor(gconst::BOID_COLOR);

        // Add the created actor to the renderer
        m_renderer->AddActor(actor);
    }

    Grid<Distribution, Dimension> & m_grid;

    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderWindow> m_render_window;
    vtkSmartPointer<vtkRenderWindowInteractor> m_render_window_interactor;
};


#endif //SWARMING_PROJECT_GRIDVISUALIZER_H
