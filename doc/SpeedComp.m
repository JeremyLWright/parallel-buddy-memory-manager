
function value_mean = separate_and_average(data)
    maxes = max(data', [], 1);
    mins = min(data', [], 1);
    data = setdiff(data', maxes, 'rows')';
    data = setdiff(data', mins, 'rows')';
    value_mean = mean(data');
endfunction

load MutexAllocSpeed.dat;
load SpinAllocSpeed.dat;
load PlatformSpeed.dat;

mutex = separate_and_average(MutexAllocSpeed(:,2:end)') *1000.0;
spin = separate_and_average(SpinAllocSpeed(:,2:end)') *1000.0;
platform = separate_and_average(PlatformSpeed(:,2:end)') *1000.0; 

cc = copper();
bar([mutex spin platform], 'facecolor', cc(40,:));
set (gca, "yticklabel", cellstr (num2str (get (gca, "ytick")(:), "%.2f")))
set(gca, 'xticklabelmode', 'manual');
set(gca, 'xtick', [1 2 3]);
set(gca, 'xticklabel', ["Mutex Locks"; "Spin Locks"; "new()"]);
print -dpng 'SpeedComparisons.png'






