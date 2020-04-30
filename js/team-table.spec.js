'use strict';
describe('team-table', function() {
  var $scope;
  var $compile;
  var element;
  var routePrefix;
  var TimeUtils;
  var $document;
  var clickFn;

  beforeEach(function() {
    module('bandcampApp');
    module('bandcampApp.mocks');

    module(function($provide) {
      routePrefix = jasmine.createSpy();
      $provide.value('routePrefix', routePrefix);

      TimeUtils = {
        getUTC: jasmine.createSpy()
      };
      $provide.value('TimeUtils', TimeUtils);
    });

    inject(function(_$rootScope_, _$compile_, _$templateCache_, _$document_) {
      $scope = _$rootScope_.$new();
      $compile = _$compile_;
      _$templateCache_.put(
        'src/manager/hire-team/team-table/team-table.tpl.html',
        '<div></div>'
      );

      $document = _$document_;
      $document.bind = jasmine.createSpy();
    });

    $document.bind.and.callFake(function(event, fn) {
      clickFn = fn;
    });
  });

  describe('initialization', function() {
    it('should be initialized successfully', function() {
      // Arrange
      element = angular.element(
        '<team-table assignments="assignments" seat-controls="seatControls"></team-table>'
      );
      $compile(element)($scope);

      // Act
      $scope.$digest();

      // Assert
      expect(element).toBeDefined();
    });
  });

  describe('mpmApplicationView', function() {
    var scope;
    var controller;
    beforeEach(function() {
      element = angular.element(
        '<team-table assignments="assignments" seat-controls="seatControls"></team-table>'
      );
      $compile(element)($scope);
      $scope.$digest();
      scope = element.isolateScope();
      controller = element.controller('teamTable');
    });
    describe('open', function() {
      it('should set scope.selectedMember', function() {
        // Arrange
        var assignment = { selection: { marketplaceMember: { id: 1 } } };

        // Act
        controller.mpmApplicationView.open(assignment);

        // Assert
        expect(scope.selectedMember).toEqual(
          assignment.selection.marketplaceMember
        );
      });
    });

    describe('close', function() {
      it('should set scope.selectedMember to null', function() {
        // Arrange

        // Act
        controller.mpmApplicationView.close();

        // Assert
        expect(scope.selectedMember).toBe(null);
      });
    });

    describe('switchMpm', function() {
      it('should set scope.selectedMember to next item in assignments when dir is 1', function() {
        // Arrange
        var dir = 1;
        scope.selectedMember = { id: 1 };
        var nextMember = { id: 2 };
        controller.assignments = [
          { selection: { marketplaceMember: { id: 1 } } },
          { selection: { marketplaceMember: nextMember } }
        ];

        // Act
        controller.mpmApplicationView.switchMpm(dir);

        // Assert
        expect(scope.selectedMember).toEqual(nextMember);
      });

      it('should not change scope.selectedMember to next item in assignments when dir is 1 and next item is not present', function() {
        // Arrange
        var dir = 1;
        var member = { id: 1 };
        scope.selectedMember = member;
        controller.assignments = [{ selection: { marketplaceMember: member } }];

        // Act
        controller.mpmApplicationView.switchMpm(dir);

        // Assert
        expect(scope.selectedMember).toEqual(member);
      });
    });

    describe('switchMpm', function() {
      it('should set scope.selectedMember to next item in assignments when dir is 1', function() {
        // Arrange
        var dir = 1;
        scope.selectedMember = { id: 1 };
        var nextMember = { id: 2 };
        controller.assignments = [
          { selection: { marketplaceMember: { id: 1 } } },
          { selection: { marketplaceMember: nextMember } }
        ];

        // Act
        controller.mpmApplicationView.switchMpm(dir);

        // Assert
        expect(scope.selectedMember).toEqual(nextMember);
      });
    });

    describe('reject', function() {
      it('should remove scope.selectedMember from controller.assignments', function() {
        // Arrange
        scope.selectedMember = { id: 1 };
        controller.assignments = [
          { selection: { marketplaceMember: { id: 1 } } }
        ];

        // Act
        controller.mpmApplicationView.reject();

        // Assert
        expect(controller.assignments).not.toContain(scope.selectedMember);
      });
    });
    describe('getCurrentTime', function() {
      it('should call TimeUtils.getUTC', function() {
        // Arrange
        var location = { timeZone: { offset: 12 } };

        // Act
        controller.getCurrentTime(location);

        // Assert
        expect(TimeUtils.getUTC).toHaveBeenCalledWith(
          null,
          location.timeZone.offset
        );
      });
    });

    describe('$document click', function() {
      it('should set controller.UI.showSeatOptions to null', function() {
        // Arrange
        var event = {};
        controller.UI.showSeatOptions = true;
        scope.$apply = function(){ return true; };

        // Act
        clickFn(event);

        // Assert
        expect(controller.UI.showSeatOptions).toBe(null);
      });
    });
  });
});
