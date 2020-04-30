'use strict';

bandcampApp.directive('teamTable', [
  'routePrefix',
  '$document',
  function (routePrefix, $document) {
    var TEMPLATE_PATH = 'manager/hire-team/team-table'
      + '/team-table.tpl.html';

    function teamTableController($scope, TimeUtils, $q) {
      var vm = this;
      vm.assignments = $scope.assignments;
      vm.UI = {showSeatOptions : null};

      vm.mpmApplicationView = {
        open: function(assignment) {
          $scope.selectedMember = assignment.selection.marketplaceMember;
        },
        close: function() {
          $scope.selectedMember = null;
        },
        switchMpm: function(dir) {
          var deferred = $q.defer();
          var curr = $scope.selectedMember;
          var i;
          // Find currently selected marketplace member in the page
          _.each(vm.assignments, function (assignment, index) {
            if (assignment.selection.marketplaceMember.id === curr.id) {
              i = index;
            }
          });
          // Selecting next/prev application within same page
          if (vm.assignments[i + dir]) {
            $scope.selectedMember =
              vm.assignments[i + dir].selection.marketplaceMember;
            deferred.resolve();
          } else {
            deferred.resolve();
          }
          return deferred.promise;
        },
        reject: function() {
          var idx = _.find(vm.assignments, function(assignment) {
            return assignment.selection.marketplaceMember.id ===
              $scope.selectedMember.id;
          });

          vm.assignments.splice(idx, 1);
        }
      };

      vm.getCurrentTime = function (location) {
        var offset = null;
        if (location) {
          offset = location.timeZone.offset;
        }
        return TimeUtils.getUTC(null, offset);
      };
      // Close dropdowns
      $document.bind('click', function(event) {
        var isDropdown =
              angular.element('.seat-person').find(event.target).length > 0;
        if(!isDropdown) {
          vm.UI.showSeatOptions = null;
          $scope.$apply();
        }
      });
    }

    return {
      restrict: 'E',
      scope: {
        assignments: '=',
        seatControls: '@'
      },
      replace: true,
      controller: teamTableController,
      controllerAs: 'vm',
      templateUrl: routePrefix + TEMPLATE_PATH
    };
  }]);
