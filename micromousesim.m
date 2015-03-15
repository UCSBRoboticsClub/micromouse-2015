%%
% Distance traveled into the room before choosing next room
indist = 0.1;

% Initial conditions
X0 = [0.5, -1, pi/2];
V0 = [1, 0];
Xt = X0 + [0, indist, 0];
inroom = 1;

% Get next state from current state and control inputs
Xp = @(X, C, dt) [C(1)*cos(X(3) + C(2)/2*dt)*dt, C(1)*sin(X(3) + C(2)/2*dt)*dt, C(2)*dt];

% Robot parameters
kp = 30;
Vmax = [1, 6];

% Direction indicator length
al = 0.1;

% Setup
dt = 0.01;
X = X0;
V = V0;
th = [];
dth = [];
plotper = 1/30;
lastplot = -inf;


% Simulation loop
for t = 0:dt:20
    % Current position + noise, target position
    p0 = X(end, :) + (rand(1, 3)-.5).*(rand(1, 3) > 0.8)*0.0;
    p1 = Xt(1:2);
    
    % Get tangent direction for arc between current point and target point
    thp = atan2(p1(2) - p0(2), p1(1) - p0(1));
    th(end+1) = 2*thp - Xt(3);
    
    % Proportional feedback on direction, only move forward when direction
    % is correct
    dth(end+1) = mod(th(end) - p0(3) + pi, 2*pi) - pi;
    Vnew = [0, 0];
    Vnew(1) = max(Vmax(1)*cos(dth(end)), 0);
    Vnew(2) = min(max(kp*dth(end), -Vmax(2)), Vmax(2));
    
    % New state
    V(end+1, :) = Vnew + Vmax.*(rand(1, 2)-.5)*0.0;
    X(end+1, :) = X(end, :) + Xp(X(end, :), V(end, :), dt);
    
    % Update target point when current target reached
    if sqrt(sum((p1 - p0(1:2)).^2)) < 0.01
        r = ceil(rand()*3);
        R = [cos(Xt(3)), -sin(Xt(3)); sin(Xt(3)), cos(Xt(3))];
        if inroom
            switch r
                case 1
                    Xt = Xt + [(R*[0.5-indist; 0.5])', pi/2];
                case 2
                    Xt = Xt + [(R*[1-indist; 0])', 0];
                case 3
                    Xt = Xt + [(R*[0.5-indist; -0.5])', -pi/2];
            end
            inroom = 0;
        else
            Xt = Xt + [(R*[indist; 0])', 0];
            inroom = 1;
        end
    end
    
    % Plot simulation
    if t > lastplot + plotper
        plot(X(:, 1), X(:, 2), 'b-', ...
            X(end, 1), X(end, 2), 'b.', ...
            [X(end, 1); X(end, 1) + al*cos(X(end, 3))], [X(end, 2), X(end, 2) + al*sin(X(end, 3))], 'b-', ...
            X(end, 1) + al*cos(X(end, 3)), X(end, 2) + al*sin(X(end, 3)), 'bx', ...
            Xt(end, 1), Xt(end, 2), 'r.', ...
            'MarkerSize', 20);
        axis square;
        axis([-4, 4, -4, 4]);
        %axis([.9, 1.1, -0.1, 0.1]);
        grid on;
        drawnow;
        lastplot = t;
        pause(plotper);
    end
end
