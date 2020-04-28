using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.Fakes;
using System.Diagnostics.CodeAnalysis;
using System.Globalization;
using System.Threading;
using System.Windows.Forms;
using CdcSoftware.Pivotal.Engine.Client.Services.Interfaces.Fakes;
using CdcSoftware.Pivotal.Engine.Common.Fakes;
using CdcSoftware.Pivotal.Engine.DataServices.Fakes;
using CdcSoftware.Pivotal.Engine.DataServices.ServerProxy.Fakes;
using CdcSoftware.Pivotal.Engine.ServerProxy.Fakes;
using CdcSoftware.Pivotal.Engine.Types.Database.Fakes;
using CdcSoftware.Pivotal.Engine.Types.Searches;
using CdcSoftware.Pivotal.Engine.Types.Searches.Fakes;
using CdcSoftware.Pivotal.WindowsFormControls;
using CdcSoftware.Pivotal.WindowsFormControls.Fakes;
using Crmc.Foundation.Common.Fakes;
using Microsoft.QualityTools.Testing.Fakes;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using NUnit.Framework;
using Shouldly;
using ShimCancelEventArgs = System.ComponentModel.Fakes.ShimCancelEventArgs;
using ShimPersonalizableControl = Crmc.CommonControls.Fakes.ShimPersonalizableControl;
using ShimPowerGrid = Crmc.CommonControls.Fakes.ShimPowerGrid;
using TestContext = NUnit.Framework.TestContext;

namespace Crmc.CommonControls.Tests
{
    [TestFixture, ExcludeFromCodeCoverage, NUnit.Framework.Category("HUT Template Tool version 3.5.10")]
    public class PowerGridTests
    {
        private const string MethodNameGetSearchResultsList = "GetSearchResultsList";
        private const string MethodNameGetSearchResults = "GetSearchResults";
        private const string MethodNameOnDataBindingComplete = "OnDataBindingComplete";
        private const string MethodNameOnSelectionChanged = "OnSelectionChanged";
        private const string MethodNameOnItemDoubleClicked = "OnItemDoubleClicked";
        private const string MethodNameContextMenuSaveGlobalPersonlizationClick = "contextMenu_SaveGlobalPersonlizationClick";
        private const string MethodNameContextMenuResetPersonlizationClick = "contextMenu_ResetPersonlizationClick";
        private const string MethodNameContextMenuResetCurrentUsersPersonlizationClick = "contextMenu_ResetCurrentUsersPersonlizationClick";
        private const string MethodNameContextMenuSavePersonlizationClick = "contextMenu_SavePersonlizationClick";
        private IDisposable _shimContext;
        private PrivateType _privateType;
        private CultureInfo _currentCulture;
        private PrivateObject _privateObject;
        private PowerGrid _testObject;
        private List<IDisposable> _disposableObjects;
        private IList<IStructuralEquatable> _methodInvocationsList;

        [OneTimeSetUp]
        public void OneTimeInitialize()
        {
            _currentCulture = Thread.CurrentThread.CurrentCulture;
            Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
            _disposableObjects = new List<IDisposable>();
        }

        [SetUp]
        public void Initialize()
        {
            _shimContext = ShimsContext.Create();
            _methodInvocationsList = new List<IStructuralEquatable>();

            ShimPersonalizableControl.QueryAccessGet = () => new ShimCommonQueryAccess
            {
                QueryGet = () => new StubIQueryProvider()
            };

            ShimPivotalMessageBox.ShowString = text =>
            {
                _methodInvocationsList.Add(
                    Tuple.Create(nameof(ShimPivotalMessageBox.ShowString),
                        text));

                return DialogResult.Yes;
            };

            ShimPivotalMessageBox.ShowStringMessageBoxButtonsMessageBoxIcon = (text, buttons, icons) =>
            {
                _methodInvocationsList.Add(
                    Tuple.Create(
                        nameof(ShimPivotalMessageBox.ShowStringMessageBoxButtonsMessageBoxIcon),
                        text));

                return DialogResult.Yes;
            };

            ShimPivotalComponentLocalizationHelper.AddLocalizablePropertiesTypeDescriptorsTypeListOfString =
                (type, properties) =>
                {
                    properties.ShouldNotBeNull();

                    var propertiesJoin = string.Join("|", properties);

                    _methodInvocationsList.Add(
                        Tuple.Create(
                            nameof(ShimPivotalComponentLocalizationHelper.AddLocalizablePropertiesTypeDescriptorsTypeListOfString),
                            propertiesJoin));

                };

            ShimControlDesignerInfo.AddComponentTypeInfoTypeStringArrayStringArray = 
                (type, properties, events) =>
                {
                    this.ShouldSatisfyAllConditions(
                        () => properties.ShouldNotBeNull(),
                        () => events.ShouldNotBeNull());

                    var propertiesJoin = string.Join("|", properties);
                    var eventsJoin = string.Join("|", events);

                    _methodInvocationsList.Add(
                        Tuple.Create(
                            nameof(ShimControlDesignerInfo.AddComponentTypeInfoTypeStringArrayStringArray),
                            propertiesJoin,
                            eventsJoin));
                };

            _testObject = new PowerGrid();
            _privateType = new PrivateType(typeof(PowerGrid));
            _privateObject = new PrivateObject(_testObject, _privateType);
        }

        [TearDown]
        public void CleanUp()
        {
            _shimContext?.Dispose();
        }

        [OneTimeTearDown]
        public void OneTimeCleanUp()
        {
            Thread.CurrentThread.CurrentCulture = _currentCulture;
            _disposableObjects.ForEach(o => o?.Dispose());
        }

        [Test]
        public void DataAccess_GetValue_ReturnsObject()
        {
            // Arrange
            _privateType.SetStaticFieldOrProperty("dataAccess", null);
            var dataAccessService = new StubIDataAccessService();
            var apiService = new StubIClientAPIService();
            apiService.GetServiceOf1(() =>
            {
                _methodInvocationsList.Add(Tuple.Create(nameof(StubIClientAPIService.GetServiceOf1)));
                return dataAccessService;
            });
            ShimClientContext.ClientAPIServiceGet = () => apiService;

            // Act
            _privateObject.GetFieldOrProperty("DataAccess");

            // Assert
            _methodInvocationsList.ShouldBeEmpty();
        }

        [TestCase("SelectedRows")]
        [TestCase("Rows")]
        public void SelectedRows_GetValue_ReturnsObject(string property)
        {
            // Arrange, Act
            var result = _privateObject.GetFieldOrProperty(property);

            // Assert
            result.ShouldNotBeNull();
        }

        [TestCase("DelayLoadData", true)]
        [TestCase("DataSource", true)]
        public void Properties_SetAndGet_ReturnsTheSameValue(
            string propertyName,
            object setValue)
        {
            // Arrange
            _privateObject.SetProperty(
                propertyName,
                setValue);

            // Act
            var result = _privateObject.GetProperty(propertyName);

            // Assert
            result.ShouldBe(setValue);
        }

        [TestCase(false, false)]
        [TestCase(false, true)]
        [TestCase(true, false)]
        [TestCase(true, true)]
        public void PostDataBindingInitialize_GivenData_InvokeMethods(
            bool delayLoadDataTestArg,
            bool isInitializedTestArg)
        {
            // Arrange
            ShimPowerGrid.AllInstances.DelayLoadDataGet = _ =>
            {
                _methodInvocationsList.Add(Tuple.Create(nameof(ShimPowerGrid.DelayLoadDataGet)));
                return delayLoadDataTestArg;
            };
            _privateObject.SetFieldOrProperty("isInitialized", isInitializedTestArg);

            ShimPowerGrid.AllInstances.RefreshGridData = _
                => _methodInvocationsList.Add(Tuple.Create(nameof(ShimPowerGrid.RefreshGridData)));

            // Act
            _testObject.PostDataBindingInitialize();

            // Assert
            _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(1);
            _methodInvocationsList.ShouldContain(Tuple.Create("DelayLoadDataGet"));

            if (TestContext.CurrentContext.Test.Name.StartsWith("PostDataBindingInitialize_GivenData_InvokeMethods(False,False"))
            {
                _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(2);
                _methodInvocationsList.ShouldContain(Tuple.Create("RefreshGridData"));
            }

            if (TestContext.CurrentContext.Test.Name.StartsWith("PostDataBindingInitialize_GivenData_InvokeMethods(False,True"))
            {
                _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(2);
                _methodInvocationsList.ShouldContain(Tuple.Create("RefreshGridData"));
            }

            if (TestContext.CurrentContext.Test.Name.StartsWith("PostDataBindingInitialize_GivenData_InvokeMethods(True,False"))
            {
                _methodInvocationsList.ShouldHaveSingleItem();
            }

            if (TestContext.CurrentContext.Test.Name.StartsWith("PostDataBindingInitialize_GivenData_InvokeMethods(True,True"))
            {
                _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(2);
                _methodInvocationsList.ShouldContain(Tuple.Create("RefreshGridData"));
            }
        }

        [TestCase(false)]
        [TestCase(true)]
        public void GetSearchResultsList_Condition_Result(bool isNullTestArg)
        {
            // Arrange
            ShimFilterPropertiesType.AllInstances.SearchResultsListPropertiesGet = _
                => isNullTestArg
                    ? null
                    : new SearchResultsListPropertiesType
                    {
                        SearchResultsListId = "0x0109030405060708"
                    };

            ShimFilterPropertiesType.AllInstances.FilterIdGet = _ => "0x0909030405060708";

            var userProfile = new ShimUserProfile();

            userProfile.GetMetaItemForItemOf2Id<SystemSearchResultsList, SystemSearch>(
                id =>
                {
                    _methodInvocationsList.Add(Tuple.Create(nameof(ShimUserProfile.GetMetaItemForItemOf2Id)));

                    return new ShimSystemSearchResultsList().Instance;
                });

            userProfile.GetMetaItemOf1Id(id =>
            {
                _methodInvocationsList.Add(Tuple.Create(nameof(ShimUserProfile.GetMetaItemOf1Id)));

                return new ShimSystemSearchResultsList().Instance;
            });

            userProfile.NewMetaItemOf1<UserSearchResultsList>(() =>
            {
                _methodInvocationsList.Add(Tuple.Create(nameof(ShimUserProfile.NewMetaItemOf1)));
                return null;
            });

            ShimDataService.AllInstances.LoadDataSearchCriteria = (_, criteria) =>
            {
                _methodInvocationsList.Add(Tuple.Create(nameof(ShimDataService.LoadDataSearchCriteria)));
                return new ShimDataTable();
            };

            ShimPowerGrid.AllInstances.DataAccessGet = _ => new StubIDataAccessService
            {
                SystemClientGet = () => new ShimSystemClient
                {
                    UserProfileGet = () => userProfile,
                    DataServiceGet = () => new ShimClientDataService()
                }
            };

            // Act
            _privateObject.Invoke(MethodNameGetSearchResultsList);

            // Assert
            _methodInvocationsList
                .ShouldHaveSingleItem()
                .ShouldBe(
                    isNullTestArg
                        ? Tuple.Create("GetMetaItemForItemOf2Id")
                        : Tuple.Create("GetMetaItemOf1Id"));
        }

        [TestCase(FilterPropertiesType.FilterParameterTypeEnum.FieldName)]
        [TestCase(FilterPropertiesType.FilterParameterTypeEnum.StringLiteral)]
        public void GetSearchResults_GivenData_InvokeMethods(FilterPropertiesType.FilterParameterTypeEnum type)
        {
            // Arrange
            var srlMetaTestArg = new ShimSystemSearchResultsList().Instance;
            var formView = new StubIFormView
            {
                FormDataGet = () => new StubIFormData
                {
                    DataSetGet = () =>
                    {
                        var dataSet = new DataSet();
                        _disposableObjects.Add(dataSet);

                        var dataTable = new DataTable
                        {
                            Columns =
                            {
                                "col1"
                            },
                            Rows = {
                                {
                                    "row1"
                                }
                            }
                        };

                        _disposableObjects.Add(dataTable);
                        dataSet.Tables.Add(dataTable);

                        return dataSet;
                    }
                }
            };

            _privateObject.SetFieldOrProperty("formView", formView);

            ShimSearch.AllInstances.SearchParametersGet = _ => new ShimSearchParameterCollection
            {
                CountGet = () => 1
            };

            var userProfile = new ShimUserProfile();

            userProfile.GetMetaItemOf1Id(id => new ShimTable().Instance);
            userProfile.GetMetaItemOf1Id(id => new ShimSystemSearch().Instance);

            ShimDataService.AllInstances.LoadDataSearchCriteria = (_, criteria) =>
            {
                _methodInvocationsList.Add(Tuple.Create(nameof(ShimDataService.LoadDataSearchCriteria)));
                return new ShimDataTable();
            };

            ShimPowerGrid.AllInstances.DataAccessGet = _ => new StubIDataAccessService
            {
                SystemClientGet = () => new ShimSystemClient
                {
                    UserProfileGet = () => userProfile,
                    DataServiceGet = () => new ShimClientDataService()
                }
            };

            ShimFilterPropertiesType.AllInstances.FilterParametersGet = _ => "col1";
            ShimFilterPropertiesType.AllInstances.FilterParametersTypeGet = _ => type;

            var parameters = new List<object>();
            ShimSearch.AllInstances.CreateSearchCriteria = _ =>
            {
                _methodInvocationsList.Add(Tuple.Create(nameof(ShimSearch.CreateSearchCriteria)));
                return new ShimSearchCriteria
                {
                    ParameterValuesGet = () => parameters
                };
            };

            // Act
            _privateObject.Invoke(MethodNameGetSearchResults, srlMetaTestArg);

            // Assert
            _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(2);
            this.ShouldSatisfyAllConditions(
                () => parameters.ShouldHaveSingleItem(),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("CreateSearchCriteria")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("LoadDataSearchCriteria")));
        }

        [Test]
        public void OnDataBindingComplete_Condition_Result()
        {
            // Arrange
            const ListChangedType ChangeTypeTestArg = ListChangedType.ItemAdded;

            _testObject.DataBindingComplete += (sender, args) =>
            {
                _methodInvocationsList.Add(Tuple.Create("DataBindingComplete"));
            };

            // Act
            _privateObject.Invoke(MethodNameOnDataBindingComplete, ChangeTypeTestArg);

            // Assert
            _methodInvocationsList
                .ShouldHaveSingleItem()
                .ShouldBe(Tuple.Create("DataBindingComplete"));
        }

        [TestCase(false)]
        [TestCase(true)]
        public void OnSelectionChanged_GivenData_InvokeMethods(bool blockSelectionChangedTestArg)
        {
            // Arrange
            _privateObject.SetFieldOrProperty("BlockSelectionChanged", blockSelectionChangedTestArg);

            _testObject.SelectionChanged += (sender, args) =>
            {
                _methodInvocationsList.Add(Tuple.Create("SelectionChanged"));
            };

            // Act
            _privateObject.Invoke(MethodNameOnSelectionChanged);

            // Assert
            if (blockSelectionChangedTestArg)
            {
                _methodInvocationsList.ShouldBeEmpty();
            }
            else
            {
                _methodInvocationsList
                    .ShouldHaveSingleItem()
                    .ShouldBe(Tuple.Create("SelectionChanged"));
            }

        }

        [Test]
        public void OnItemDoubleClicked_Condition_Result()
        {
            // Arrange
            ShimCancelEventArgs.Constructor = _
                => _methodInvocationsList.Add(
                    Tuple.Create(nameof(ShimCancelEventArgs.Constructor)));

            _testObject.ItemDoubleClick += (sender, args) =>
            {
                _methodInvocationsList.Add(Tuple.Create("ItemDoubleClick"));
            };

            // Act
            _privateObject.Invoke(MethodNameOnItemDoubleClicked);

            // Assert
            _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(2);
            this.ShouldSatisfyAllConditions(
                () => _methodInvocationsList.ShouldContain(Tuple.Create("Constructor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("ItemDoubleClick")));
        }

        [Test]
        public void SaveLayout_OnValidCall_ShouldNotThrowException()
        {
            // Arrange
            const bool IsGlobalTestArg = true;

            // Act
            _testObject.SaveLayout(IsGlobalTestArg);

            // Assert
            _methodInvocationsList.ShouldBeEmpty();
        }

        [TestCase("Initialize")]
        [TestCase("RefreshGridData")]
        [TestCase("ResetLayout")]
        public void Method_OnValidCall_ShouldNotThrowException(string method)
        {
            // Arrange, Act
            _privateObject.Invoke(method);

            // Assert
            _methodInvocationsList.ShouldBeEmpty();
        }

        [Test]
        public void ContextMenuSaveGlobalPersonlizationClick_Condition_Result()
        {
            // Arrange
            var senderTestArg = new object();
            var eTestArg = EventArgs.Empty;

            ShimPowerGrid.AllInstances.SaveLayoutBoolean = (_, isGlobal)
                => _methodInvocationsList.Add(
                    Tuple.Create(nameof(ShimPowerGrid.SaveLayoutBoolean), isGlobal));

            // Act
            _privateObject.Invoke(
                MethodNameContextMenuSaveGlobalPersonlizationClick,
                senderTestArg,
                eTestArg);

            // Assert
            _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(2);
            this.ShouldSatisfyAllConditions(
                () => _methodInvocationsList.ShouldContain(Tuple.Create("ShowStringMessageBoxButtonsMessageBoxIcon", "This will save a new default layout for all users who do not have a saved user layout and overwrite any existing layout. Okay to proceed?")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("SaveLayoutBoolean", true)));
        }

        [Test]
        public void ContextMenuResetPersonlizationClick_Condition_Result()
        {
            // Arrange
            var senderTestArg = new object();
            var eTestArg = EventArgs.Empty;

            ShimPersonalizableControl.AllInstances.RemovePersonalizedData = _
                => _methodInvocationsList.Add(Tuple.Create(nameof(ShimPersonalizableControl.RemovePersonalizedData)));

            ShimPowerGrid.AllInstances.RefreshGridData = _
                => _methodInvocationsList.Add(Tuple.Create(nameof(ShimPowerGrid.RefreshGridData)));

            // Act
            _privateObject.Invoke(
                MethodNameContextMenuResetPersonlizationClick,
                senderTestArg,
                eTestArg);

            // Assert
            _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(3);
            this.ShouldSatisfyAllConditions(
                () => _methodInvocationsList.ShouldContain(Tuple.Create("ShowStringMessageBoxButtonsMessageBoxIcon", "This will remove only the global default layout for this form and control. Okay to proceed?")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("RemovePersonalizedData")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("RefreshGridData")));
        }

        [Test]
        public void ContextMenuResetCurrentUsersPersonlizationClick_Condition_Result()
        {
            // Arrange
            var senderTestArg = new object();
            var eTestArg = EventArgs.Empty;
            ShimPersonalizableControl.AllInstances.RemoveCurrentUsersPersonalizedData = _
                => _methodInvocationsList.Add(
                    Tuple.Create(nameof(ShimPersonalizableControl.RemoveCurrentUsersPersonalizedData)));

            ShimPowerGrid.AllInstances.RefreshGridData = _
                => _methodInvocationsList.Add(Tuple.Create(nameof(ShimPowerGrid.RefreshGridData)));

            // Act
            _privateObject.Invoke(
                MethodNameContextMenuResetCurrentUsersPersonlizationClick,
                senderTestArg,
                eTestArg);

            // Assert
            _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(76);
            this.ShouldSatisfyAllConditions(
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|FilterProperties|Margin|AutoScaleDimensions|DelayLoadData")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Margin|AutoScaleDimensions|Enabled|Required|TabStop|ReadOnly|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Margin|AutoScaleDimensions|Enabled|Required|TabStop|ReadOnly|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|AttachmentClick|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Name|PivotalBindings|Size|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|AutoScroll|Padding|Text|Controls|AllowDrop|BorderStyle|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Name|PivotalBindings|Size|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|AutoScroll|Padding|Text|Controls|AllowDrop|BorderStyle|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ScrollBars|BorderStyle|Multiline|ReadOnly|Required|TabStop|Text|TextAlign|Margin|MaxLength|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ScrollBars|BorderStyle|Multiline|ReadOnly|Required|TabStop|Text|TextAlign|Margin|MaxLength|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ApplyFormatting|DecimalPoints|IsCurrency|Value|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ApplyFormatting|DecimalPoints|IsCurrency|Value|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|BorderStyle|ForeColor|TabIndex|Enabled|Padding|Margin|AutoScroll|AutoSize|AutoSizeMode|FlowDirection|TabStop|WrapContents|Controls")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|BorderStyle|ForeColor|TabIndex|Enabled|Padding|Margin|AutoScroll|AutoSize|AutoSizeMode|FlowDirection|TabStop|WrapContents|Controls", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|ForeColor|BorderStyle|CheckBoxes|Enabled|FullRowSelect|HideSelection|HotTracking|ImageKey|SelectedImageKey|ShowLines|ShowNodeToolTips|ShowPlusMinus|ItemHeight|LabelEdit|LineColor|NodeTemplates|DelayLoadData")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|ForeColor|BorderStyle|CheckBoxes|Enabled|FullRowSelect|HideSelection|HotTracking|ImageKey|SelectedImageKey|ShowLines|ShowNodeToolTips|ShowPlusMinus|ItemHeight|LabelEdit|LineColor|NodeTemplates|DelayLoadData", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|BeforeCheck|AfterCheck|BeforeCollapse|AfterCollapse|BeforeExpand|AfterExpand|BeforeSelect|AfterSelect|ItemDrag|NodeMouseClick|NodeMouseDoubleClick|NodeMouseHover|NodeInitialize|NodeNewRecord|NodeOpenRecord|NodeDeleteRecord")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|ReadOnly|Required|TabStop|Text|TextAlign|Mask|Margin|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|ReadOnly|Required|TabStop|Text|TextAlign|Mask|Margin|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|MaskInputRejected|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ReadOnly|Required|TabStop|TextAlign|Url|Margin|MaxLength|AutoScaleDimensions|EditControlBackColor|EditControlForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ReadOnly|Required|TabStop|TextAlign|Url|Margin|MaxLength|AutoScaleDimensions|EditControlBackColor|EditControlForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|UrlIconClick|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|Margin|Required|TabStop|TimeValue|ReadOnly|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|Margin|Required|TabStop|TimeValue|ReadOnly|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|TimeValueChanged|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Value|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|AutoScaleDimensions")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Value|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|AutoScaleDimensions", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|KeyIconClick|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Items|Enabled|ButtonAlign|LayoutStyle|Required|Value|Margin|AutoScaleDimensions|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Items|Enabled|ButtonAlign|LayoutStyle|Required|Value|Margin|AutoScaleDimensions|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|ValueChanged|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|MaximumFileSize|ImageAutoLoad|BackgroundImageLayout|AllowSaveImageToFile|AllowImageLayoutChange|Enabled|AutoScaleDimensions")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|MaximumFileSize|ImageAutoLoad|BackgroundImageLayout|AllowSaveImageToFile|AllowImageLayoutChange|Enabled|AutoScaleDimensions", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|LoadImageData|SetImageData|SaveImageToFile|ClearImage")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|ReadOnly|Required|TabStop|BinaryRtf|Margin|BackColor|ForeColor|Rtf|ScrollBars|WordWrap")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|ReadOnly|Required|TabStop|BinaryRtf|Margin|BackColor|ForeColor|Rtf|ScrollBars|WordWrap", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|FormTitle|Enabled|AutoSize|AutoSizeMode|AutoScroll|Padding|AutoScaleDimensions|Controls|Language|BackColor|ForeColor|ScalingFactor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|FormTitle|Enabled|AutoSize|AutoSizeMode|AutoScroll|Padding|AutoScaleDimensions|Controls|Language|BackColor|ForeColor|ScalingFactor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|Margin|CalendarFont|CalendarForeColor|CalendarMonthBackground|CalendarTitleBackColor|CalendarTitleForeColor|CalendarTrailingForeColor|Format|Required|TabStop|DateValue|ReadOnly|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|Margin|CalendarFont|CalendarForeColor|CalendarMonthBackground|CalendarTitleBackColor|CalendarTitleForeColor|CalendarTrailingForeColor|Format|Required|TabStop|DateValue|ReadOnly|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|CloseUp|DateValueChanged|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|Margin|TabStop|TextAlign|TimeValue|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|Margin|TabStop|TextAlign|TimeValue|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ReadOnly|EditControlBackColor|EditControlForeColor|ForeignKeyFilter|Enabled|Required|TabStop|Value|Margin|SortAscending|LinesPerPage|AutoScaleDimensions|SpyglassVisible|NavigationButtonVisible|SpyglassEnabled|BackColor|ForeColor|TypeAheadTimeout|AllowWidthResize|PreventListLoading|NavigationButtonEnabled")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ReadOnly|EditControlBackColor|EditControlForeColor|ForeignKeyFilter|Enabled|Required|TabStop|Value|Margin|SortAscending|LinesPerPage|AutoScaleDimensions|SpyglassVisible|NavigationButtonVisible|SpyglassEnabled|BackColor|ForeColor|TypeAheadTimeout|AllowWidthResize|PreventListLoading|NavigationButtonEnabled", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|FKIconClick|DataSourceUpdated|CommittingNewValue|SpyglassIconClick")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|BorderStyle|ForeColor|TabIndex|Enabled|Margin|Padding|AutoScroll|AutoSize|AutoSizeMode|BackgroundImageName|BackgroundImageLayout|TabStop|Controls")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|BorderStyle|ForeColor|TabIndex|Enabled|Margin|Padding|AutoScroll|AutoSize|AutoSizeMode|BackgroundImageName|BackgroundImageLayout|TabStop|Controls", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|ForeColor|TabStop|DataSegment|AllowUserToOrderColumns|AllowUserToResizeColumns|AllowUserToResizeRows|AutoSizeColumnsMode|AutoSizeRowsMode|ColumnHeadersBorderStyle|ColumnHeadersHeight|ColumnHeadersHeightSizeMode|ColumnHeadersVisible|ColumnHeadersDefaultCellStyle|DefaultCellStyle|GridColor|MultiSelect|RowsDefaultCellStyle|RowHeadersBorderStyle|RowHeadersDefaultCellStyle|RowHeadersWidth|RowHeadersWidthSizeMode|RowHeadersVisible|ScrollBars|Columns|AlternatingRowsDefaultCellStyle|BackgroundColor|BorderStyle|Style|Margin")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|ForeColor|TabStop|DataSegment|AllowUserToOrderColumns|AllowUserToResizeColumns|AllowUserToResizeRows|AutoSizeColumnsMode|AutoSizeRowsMode|ColumnHeadersBorderStyle|ColumnHeadersHeight|ColumnHeadersHeightSizeMode|ColumnHeadersVisible|ColumnHeadersDefaultCellStyle|DefaultCellStyle|GridColor|MultiSelect|RowsDefaultCellStyle|RowHeadersBorderStyle|RowHeadersDefaultCellStyle|RowHeadersWidth|RowHeadersWidthSizeMode|RowHeadersVisible|ScrollBars|Columns|AlternatingRowsDefaultCellStyle|BackgroundColor|BorderStyle|Style|Margin", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|SecondarySortClick")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|Margin|Padding|Checked|Required|TabStop|ReadOnly|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|Margin|Padding|Checked|Required|TabStop|ReadOnly|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|CheckedChanged|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ApplyFormatting|PhoneNumber|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|AutoScaleDimensions|EditControlBackColor|EditControlForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ApplyFormatting|PhoneNumber|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|AutoScaleDimensions|EditControlBackColor|EditControlForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|DialIconClick|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Margin|Padding|BorderColor|ButtononClickColor|HotBackColor|Style|AutoSize|AutoSizeMode|Enabled|FlatStyle|TabStop|Text|TextAlign|LegacyButtonName|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Margin|Padding|BorderColor|ButtononClickColor|HotBackColor|Style|AutoSize|AutoSizeMode|Enabled|FlatStyle|TabStop|Text|TextAlign|LegacyButtonName|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|Text|TextAlign|Enabled|Margin|Padding|BackColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|Text|TextAlign|Enabled|Margin|Padding|BackColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|LinkClicked")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|AllowDrop|BackColor|BorderStyle|ForeColor|TabIndex|Enabled|Margin|AutoScaleDimensions|FixedPanel|IsSplitterFixed|Orientation|Panel1|Panel1Collapsed|Panel1MinSize|Panel2|Panel2Collapsed|Panel2MinSize|SplitterDistance|SplitterIncrement|SplitterWidth|TabStop")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|AllowDrop|BackColor|BorderStyle|ForeColor|TabIndex|Enabled|Margin|AutoScaleDimensions|FixedPanel|IsSplitterFixed|Orientation|Panel1|Panel1Collapsed|Panel1MinSize|Panel2|Panel2Collapsed|Panel2MinSize|SplitterDistance|SplitterIncrement|SplitterWidth|TabStop", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|SplitterMoved|SplitterMoving")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|Text|TextAlign|Margin|Padding|AutoSize|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BorderStyle|Text|TextAlign|Margin|Padding|AutoSize|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|Margin|AllowWidthResize|MaxLength|ReadOnly|Value|DropDownHeight|DropDownStyle|DropDownWidth|FlatStyle|Items|MaxDropDownItems|Required|Sorted|TabStop|Text|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Enabled|Margin|AllowWidthResize|MaxLength|ReadOnly|Value|DropDownHeight|DropDownStyle|DropDownWidth|FlatStyle|Items|MaxDropDownItems|Required|Sorted|TabStop|Text|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|DropDown|DropDownClosed|SelectedIndexChanged|DataSourceUpdated|CommittingNewValue|SelectedValueChanged|SelectionChangeCommitted|ValueMemberChanged")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|EditInPlace|Columns|AlternatingRowsDefaultCellStyle|BackgroundColor|BorderStyle|Style|Margin|DataSegment|AddInPlace|AllowUserToAddRows|AllowUserToOrderColumns|AllowUserToDeleteRows|AllowUserToResizeColumns|AllowUserToResizeRows|AutoSizeColumnsMode|AutoSizeRowsMode|ColumnHeadersBorderStyle|ColumnHeadersHeight|ColumnHeadersHeightSizeMode|ColumnHeadersVisible|ColumnHeadersDefaultCellStyle|DefaultCellStyle|GridColor|MultiSelect|RowsDefaultCellStyle|RowHeadersBorderStyle|RowHeadersDefaultCellStyle|RowHeadersWidth|RowHeadersWidthSizeMode|RowHeadersVisible|ScrollBars|TabStop|BackColor|ForeColor|EditMode")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|EditInPlace|Columns|AlternatingRowsDefaultCellStyle|BackgroundColor|BorderStyle|Style|Margin|DataSegment|AddInPlace|AllowUserToAddRows|AllowUserToOrderColumns|AllowUserToDeleteRows|AllowUserToResizeColumns|AllowUserToResizeRows|AutoSizeColumnsMode|AutoSizeRowsMode|ColumnHeadersBorderStyle|ColumnHeadersHeight|ColumnHeadersHeightSizeMode|ColumnHeadersVisible|ColumnHeadersDefaultCellStyle|DefaultCellStyle|GridColor|MultiSelect|RowsDefaultCellStyle|RowHeadersBorderStyle|RowHeadersDefaultCellStyle|RowHeadersWidth|RowHeadersWidthSizeMode|RowHeadersVisible|ScrollBars|TabStop|BackColor|ForeColor|EditMode", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|RowEnter|SecondaryAddClick|SecondaryEditClick|SecondaryDeleteClick|SecondarySortClick|RowLeave|RowsAdded|RowsRemoved|RowValidating|RowValidated|RowHeaderMouseClick|CellContentClick|CellContentDoubleClick|CellEnter|CellLeave|CellMouseClick|CellMouseDoubleClick|CellMouseEnter|CellMouseLeave|CellMouseMove|CellMouseDown|CellMouseUp|CellValidating|CellValidated|CellValueChanged|DataError|CellContextMenuStripNeeded")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|ForeColor|TabIndex|Enabled|Margin|Padding|AutoSize|AutoSizeMode|Text|Controls")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|BackColor|ForeColor|TabIndex|Enabled|Margin|Padding|AutoSize|AutoSizeMode|Text|Controls", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|CellBorderStyle|Enabled|Margin|Padding|AutoScroll|AutoSize|AutoSizeMode|ColumnCount|ColumnStyles|GrowStyle|RowCount|RowStyles|TabStop|AllowDrop|BackColor|ForeColor|TabIndex|TableCells")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|CellBorderStyle|Enabled|Margin|Padding|AutoScroll|AutoSize|AutoSizeMode|ColumnCount|ColumnStyles|GrowStyle|RowCount|RowStyles|TabStop|AllowDrop|BackColor|ForeColor|TabIndex|TableCells", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|FilterProperties|Margin|AutoScaleDimensions|DelayLoadData", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|CellMouseClick|CellMouseDoubleClick|CellLinkClick|EnterKeyPressed|SelectionChanged")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ControlSettings|Margin|AutoScaleDimensions")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ControlSettings|Margin|AutoScaleDimensions", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Margin|AllowNavigation|IsWebBrowserContextMenuEnabled|ScriptErrorsSuppressed|ScrollBarsEnabled|TabStop|Url|WebBrowserShortcutsEnabled|BackColor|ForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|Margin|AllowNavigation|IsWebBrowserContextMenuEnabled|ScriptErrorsSuppressed|ScrollBarsEnabled|TabStop|Url|WebBrowserShortcutsEnabled|BackColor|ForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|DocumentCompleted|FileDownload|Navigated|ProgressChanged")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|SelectedIndex|Enabled|BackColor|Style|HotTrackBackColor|HotTrackTextColor|HotTrackTextFontStyle|InactiveTabBackColor|InactiveTabBackColor2|InactiveTabTextColor|SelectedTabBackColor|SelectedTabBackColor2|SelectedTabTextColor|SelectedTabTextFontStyle|TabBorderColor|TabPageBorderFillColor|Margin|Alignment|HotTrack|Multiline|TabPages|TabStop|Controls")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|SelectedIndex|Enabled|BackColor|Style|HotTrackBackColor|HotTrackTextColor|HotTrackTextFontStyle|InactiveTabBackColor|InactiveTabBackColor2|InactiveTabTextColor|SelectedTabBackColor|SelectedTabBackColor2|SelectedTabTextColor|SelectedTabTextFontStyle|TabBorderColor|TabPageBorderFillColor|Margin|Alignment|HotTrack|Multiline|TabPages|TabStop|Controls", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|Deselected|Deselecting|Selected|SelectedIndexChanged|Selecting")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ApplyFormatting|FaxNumber|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|AutoScaleDimensions|EditControlBackColor|EditControlForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|ApplyFormatting|FaxNumber|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|AutoScaleDimensions|EditControlBackColor|EditControlForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|FaxIconClick|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddLocalizablePropertiesTypeDescriptorsTypeListOfString", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|EmailAddress|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|AutoScaleDimensions|EditControlBackColor|EditControlForeColor")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|EmailAddress|ReadOnly|Required|TabStop|TextAlign|Margin|MaxLength|AutoScaleDimensions|EditControlBackColor|EditControlForeColor", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|EmailIconClick|DataSourceUpdated|CommittingNewValue")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "AutoScroll|BackColor|ForeColor|Controls", "")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("AddComponentTypeInfoTypeStringArrayStringArray", "Font|Location|Name|PivotalBindings|Size|Visible|AllowDrop|TabIndex|Anchor|Dock|EventHandlers|ControlId|Height|Width|LegacyTabName|LegacySegmentName|ToolTipText|FilterProperties|Margin|AutoScaleDimensions|DelayLoadData", "Click|DoubleClick|DragDrop|DragEnter|DragLeave|DragOver|GiveFeedback|Enter|KeyDown|KeyPress|KeyUp|Leave|MouseClick|MouseDown|MouseEnter|MouseHover|MouseLeave|MouseMove|MouseUp|MouseWheel|QueryContinueDrag|TextChanged|Validated|Validating|ItemDoubleClick|SelectionChanged|DataBindingComplete")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("ShowStringMessageBoxButtonsMessageBoxIcon", "This will remove only the current users saved layout for this form and control. Okay to proceed?")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("RemoveCurrentUsersPersonalizedData")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("RefreshGridData")));
        }

        [Test]
        public void ContextMenuSavePersonlizationClick_Condition_Result()
        {
            // Arrange
            var senderTestArg = new object();
            var eTestArg = EventArgs.Empty;

            ShimPowerGrid.AllInstances.SaveLayoutBoolean = (_, isGlobal)
                => _methodInvocationsList.Add(Tuple.Create(nameof(ShimPowerGrid.SaveLayoutBoolean), isGlobal));

            // Act
            _privateObject.Invoke(
                MethodNameContextMenuSavePersonlizationClick,
                senderTestArg,
                eTestArg);

            // Assert
            _methodInvocationsList.Count.ShouldBeGreaterThanOrEqualTo(2);
            this.ShouldSatisfyAllConditions(
                () => _methodInvocationsList.ShouldContain(Tuple.Create("ShowStringMessageBoxButtonsMessageBoxIcon", "This will save a new default layout for this user and overwrite any existing layout. Okay to proceed?")),
                () => _methodInvocationsList.ShouldContain(Tuple.Create("SaveLayoutBoolean", false)));
        }
    }
}
