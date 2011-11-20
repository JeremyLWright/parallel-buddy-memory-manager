%High Water of Waiting Requests
levels_5 = [0 0 0 2 1];
levels_10 = [0 0 0 3 2 1 1 1 1 1];
levels_15 = [0 0 0 4 2 1 1 1 1 1 1 1 1 1 1];

cc = copper();
bar(levels_5, 'facecolor', cc(40,:));
%text(5,1.8, 'Block Size = 4 bytes');
set(gca, 'xticklabelmode', 'manual');
set(gca, 'xtick', [1:5]);
set(gca, 'xticklabel', ['2^0'; '2^1'; '2^2'; '2^3'; '2^4']);
title('5-Level Allocator');
ylabel('Max Pending Requests');
xlabel('Block Level');
print -dpng 'PendingRequests5Levels.png'

bar(levels_10, 'facecolor', cc(40,:));
set(gca, 'xticklabelmode', 'manual');
set(gca, 'xtick', [1:10]);
axis([0,11]);
set(gca, 'xticklabel', ['2^0'; '2^1'; '2^2'; '2^3'; '2^4'; '2^5'; '2^6'; '2^7'; '2^8'; '2^9']);
title('10-Level Allocator');
ylabel('Max Pending Requests');
xlabel('Block Level');
print -dpng 'PendingRequests10Levels.png'

bar(levels_15, 'facecolor', cc(40,:));
set(gca, 'xticklabelmode', 'manual');
set(gca, 'xtick', [1:15]);
axis([0,16]);
set(gca, 'xticklabel', ['2^0'; '2^1'; '2^2'; '2^3'; '2^4'; '2^5'; '2^6'; '2^7';
'2^8'; '2^9'; '2^1^0'; '2^1^1'; '2^1^2'; '2^1^3'; '2^1^4']);
title('15-Level Allocator');
ylabel('Max Pending Requests');
xlabel('Block Level');
print -dpng 'PendingRequests15Levels.png'

subplot(3,1,1)
bar(levels_5, 'facecolor', cc(40,:));
%text(5,1.8, 'Block Size = 4 bytes');
set(gca, 'xticklabelmode', 'manual');
set(gca, 'xtick', [1:5]);
set(gca, 'xticklabel', ['2^0'; '2^1'; '2^2'; '2^3'; '2^4']);
title('5-Level Allocator');
ylabel('Max Pending Requests');

subplot(3,1,2)
bar(levels_10, 'facecolor', cc(40,:));
set(gca, 'xticklabelmode', 'manual');
set(gca, 'xtick', [1:10]);
axis([0,11]);
set(gca, 'xticklabel', ['2^0'; '2^1'; '2^2'; '2^3'; '2^4'; '2^5'; '2^6'; '2^7'; '2^8'; '2^9']);
title('10-Level Allocator');
ylabel('Max Pending Requests');

subplot(3,1,3)
bar(levels_15, 'facecolor', cc(40,:));
set(gca, 'xticklabelmode', 'manual');
set(gca, 'xtick', [1:15]);
axis([0,16]);
set(gca, 'xticklabel', ['2^0'; '2^1'; '2^2'; '2^3'; '2^4'; '2^5'; '2^6'; '2^7';
'2^8'; '2^9'; '2^1^0'; '2^1^1'; '2^1^2'; '2^1^3'; '2^1^4']);
title('15-Level Allocator');
ylabel('Max Pending Requests');
xlabel('Block Level');
print -dpng 'combined.png'
