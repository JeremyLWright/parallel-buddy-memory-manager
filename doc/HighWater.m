%High Water of Waiting Requests
levels_5 = [0 0 0 2 1];
levels_10 = [0 0 0 3 2 1 1 1 1 1];
levels_15 = [0 0 0 4 2 1 1 1 1 1 1 1 1 1 1];

cc = copper();
bar(levels_5, 'facecolor', cc(40,:));
title('Pending Requests on 128 byte Allocator');
ylabel('Max Waiting Requests');
xlabel('Block Level');
print -dpng 'PendingRequests5Levels.png'

bar(levels_10, 'facecolor', cc(40,:));
title('Pending Requests on 4 KByte Allocator');
ylabel('Max Waiting Requests');
xlabel('Block Level');
print -dpng 'PendingRequests10Levels.png'

bar(levels_15, 'facecolor', cc(40,:));
title('Pending Requests on 131 KByte Allocator');
ylabel('Max Waiting Requests');
xlabel('Block Level');
print -dpng 'PendingRequests15Levels.png'

