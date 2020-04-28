package com.tradebeam.gtm.test.hut.biz.alert.ejb.session;

import static java.lang.String.format;
import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static org.hamcrest.CoreMatchers.instanceOf;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.nullValue;
import static org.junit.Assert.fail;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Matchers.isA;
import static org.mockito.Matchers.isNull;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.powermock.api.mockito.PowerMockito.doAnswer;
import static org.powermock.api.mockito.PowerMockito.doThrow;
import static org.powermock.api.mockito.PowerMockito.mockStatic;
import static org.powermock.api.mockito.PowerMockito.spy;
import static org.powermock.api.mockito.PowerMockito.verifyPrivate;
import static org.powermock.api.mockito.PowerMockito.verifyStatic;
import static org.powermock.api.mockito.PowerMockito.when;
import static org.powermock.api.mockito.PowerMockito.whenNew;
import static org.powermock.reflect.Whitebox.getInternalState;
import static org.powermock.reflect.Whitebox.setInternalState;

import com.tradebeam.DAOException;
import com.tradebeam.ValueObject;
import com.tradebeam.alert.AlertDetailsVO;
import com.tradebeam.alert.AlertMessageVO;
import com.tradebeam.alert.UserAlertVO;
import com.tradebeam.alert.intf.UserAlertMessageDAO;
import com.tradebeam.alert.util.AlertColumnSortDefinition;
import com.tradebeam.common.Assert;
import com.tradebeam.common.ErrorCode;
import com.tradebeam.common.ErrorCodeType;
import com.tradebeam.common.InvalidDataException;
import com.tradebeam.common.MissingRequiredDataException;
import com.tradebeam.common.ParameterList;
import com.tradebeam.common.lang.AlertCategory;
import com.tradebeam.common.lang.AlertType;
import com.tradebeam.common.lang.BusinessName;
import com.tradebeam.common.lang.MaximumLengthExceededException;
import com.tradebeam.common.lang.OperativeStatus;
import com.tradebeam.common.lang.TimeZone;
import com.tradebeam.common.lang.UserName;
import com.tradebeam.common.object.BusinessEntity;
import com.tradebeam.common.object.DashboardItem;
import com.tradebeam.common.object.DashboardItemValue;
import com.tradebeam.common.object.PrimaryIdentifier;
import com.tradebeam.common.pagination.ColumnSortDefinition;
import com.tradebeam.common.pagination.Pagination;
import com.tradebeam.common.pagination.SortOrder;
import com.tradebeam.gtm.biz.BizException;
import com.tradebeam.gtm.biz.SystemException;
import com.tradebeam.gtm.biz.alert.ejb.session.AlertMessageBean;
import com.tradebeam.gtm.biz.checklist.ejb.session.ChecklistManagementFacade;
import com.tradebeam.gtm.common.alert.enumeration.AlertFieldSortDefinition;
import com.tradebeam.gtm.common.alert.to.AlertMessageTO;
import com.tradebeam.gtm.common.alert.to.UserAlertReqTO;
import com.tradebeam.gtm.common.alert.to.UserAlertResTO;
import com.tradebeam.gtm.common.home.to.DashboardItemCountValueTO;
import com.tradebeam.gtm.common.to.BaseTransferObject;
import com.tradebeam.gtm.common.to.BusinessEntityTO;
import com.tradebeam.gtm.common.to.UserPreferenceTO;
import com.tradebeam.gtm.common.to.UserTO;
import com.tradebeam.gtm.common.util.TimeUtils;
import com.tradebeam.gtm.common.util.pagination.PaginationParameter;
import com.tradebeam.user.UserVO;
import com.tradebeam.util.HibernateDAOProvider;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ErrorCollector;
import org.junit.runner.RunWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Captor;
import org.mockito.Mock;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;
import org.powermock.core.classloader.annotations.PrepareForTest;
import org.powermock.modules.junit4.PowerMockRunner;

@RunWith(PowerMockRunner.class)
@PrepareForTest({
        Assert.class,
        HibernateDAOProvider.class,
        TimeUtils.class,
        UserAlertVO.class,
        AlertMessageBean.class
})
public class AlertMessageBeanTest {

    private static final String LOGGER_FIELD = "logger";
    private static final String ERROR = "Error!";
    private static final long VERSION = 2002L;
    private static final OperativeStatus OPERATIVE_STATUS = OperativeStatus.ACTIVE;
    private static final long CREATED_TIME = 1234567890L;
    private static final long LAST_UPDATED_TIME = 2345678901L;
    private static final long CURRENT_TIME = 3456789012L;
    private static final long CURRENT_TIME_OTHER = 2234567890L;
    private static final long USER_ID_1 = 123L;
    private static final long USER_ID_2 = 223L;
    private static final long USER_ID_3 = 323L;
    private static final String USER_NAME_1 = "UserName1";
    private static final String USER_NAME_2 = "UserName2";
    private static final String USER_NAME_3 = "UserName3";
    private static final long BUSINESS_ID = 7890L;
    private static final String BUSINESS_NAME = "BusinessName";
    private static final long IDENTIFIER = 323L;
    private static final long IDENTIFIER_OTHER = 423L;
    private static final String MISSING_DATA_MSG = "Specified %s cannot be null!";
    private static final TimeZone TIME_ZONE = TimeZone.GMT;
    private static final int MAX_LENGTH_ALERT_FIELD_VALUE = 1000;
    private static final AlertType ALERT_TYPE = AlertType.PRODUCT_CREATED;
    private static final String ALERT_SUBJECT = "AlertSubject";
    private static final String ALERT_FROM = "AlertFrom";
    private static final AlertCategory ALERT_CATEGORY = AlertCategory.PRODUCTCATALOG;
    private static final String ALERT_MESSAGE = "AlertMessage";
    private static final long READ_TIME = 487563876584L;
    private static final String DAO_EXCEPTION_MSG = "com.tradebeam.DAOException: Error!";
    private static final int START_ROW = 10;
    private static final int NUMBER_OF_ROWS = 100;
    private static final SortOrder SORT_ORDER = SortOrder.ASCENDING;
    private static final AlertFieldSortDefinition FIELD_SORT = AlertFieldSortDefinition.CATEGORY;
    private static final AlertColumnSortDefinition COLUMN_SORT = AlertColumnSortDefinition.CATEGORY;
    private static final int COUNT = 5000;
    private static final String MESSAGE_PROPERTY = "MessageProperty";
    private static final String MESSAGE_VALUE = "MessageValue";

    @Rule
    public final ErrorCollector collector = new ErrorCollector();

    @Mock
    private UserAlertMessageDAO userAlertMessageDAO;
    @Mock
    private Logger logger;
    @Captor
    private ArgumentCaptor<?> objCaptor;

    private AlertMessageBean alertMessageBean;
    private BusinessEntityTO businessEntityTO;
    private Date currentTime;
    private PrimaryIdentifier identifier;
    private PrimaryIdentifier identifierOther;
    private UserTO userTO;
    private Date currentTimeOther;
    private Date createdTime;
    private Date lastUpdatedTime;
    private PrimaryIdentifier businessId;
    private PrimaryIdentifier userId;
    private Logger origLogger;

    @After
    public void tearDown() {
        setInternalState(AlertMessageBean.class, LOGGER_FIELD, origLogger);
    }

    @Before
    public void setUp() throws Exception {
        currentTime = new Date(CURRENT_TIME);
        currentTimeOther = new Date(CURRENT_TIME_OTHER);
        createdTime = new Date(CREATED_TIME);
        lastUpdatedTime = new Date(LAST_UPDATED_TIME);
        userId = PrimaryIdentifier.valueOf(USER_ID_1);
        businessId = PrimaryIdentifier.valueOf(BUSINESS_ID);
        identifier = PrimaryIdentifier.valueOf(IDENTIFIER);
        identifierOther = PrimaryIdentifier.valueOf(IDENTIFIER_OTHER);
        userId = PrimaryIdentifier.valueOf(USER_ID_3);
        userTO = new UserTO();
        userTO.setPrimaryId(userId);
        userTO.setUserName(USER_NAME_3);
        userTO.setUserPreference(new UserPreferenceTO());
        userTO.getUserPreference().setPreferredTimezone(TIME_ZONE.getCode());
        businessEntityTO = initBusinessEntityTO();

        mockStatic(HibernateDAOProvider.class);
        when(HibernateDAOProvider.getUserAlertMessageDAO()).thenReturn(userAlertMessageDAO);
        origLogger = getInternalState(AlertMessageBean.class, LOGGER_FIELD);
        setInternalState(AlertMessageBean.class, LOGGER_FIELD, logger);
        when(logger.isDebugEnabled()).thenReturn(true);

        alertMessageBean = new AlertMessageBean();

        spy(Assert.class);
    }

    @Test
    public void givenDeleteUserAlertWhenUserAlertVONotNullThenExpected() throws Exception {
        // Arrange
        final UserAlertVO userAlertVO = initValueObjectNew(initUserAlertVO());
        userAlertVO.setPrimaryId(identifier);
        userAlertVO.getAlertMessage().setPrimaryId(identifierOther);
        when(userAlertMessageDAO.findById(IDENTIFIER)).thenReturn(userAlertVO);
        doAnswer(updateValueObject()).when(userAlertMessageDAO).update(userAlertVO);
        whenNew(Date.class).withNoArguments().thenReturn(currentTime);
        // Act
        alertMessageBean.deleteUserAlert(IDENTIFIER);
        // Assert
        assertUserAlertVOComplete(userAlertVO, identifier, identifierOther);
        assertSavedValueObjectModified(userAlertVO);
        collector.checkThat(userAlertVO.getIsDeleted(), is(true));
        collector.checkThat(userAlertVO.getDeletedById(), is(USER_ID_1));
        collector.checkThat(userAlertVO.getDeletedTime(), is(currentTime));
        verify(userAlertMessageDAO).findById(IDENTIFIER);
        verify(userAlertMessageDAO).update(userAlertVO);
    }

    @Test
    public void givenDeleteUserAlertWhenUserAlertVONullThenExpected() throws Exception {
        // Act
        alertMessageBean.deleteUserAlert(IDENTIFIER);
        // Assert
        verify(userAlertMessageDAO).findById(IDENTIFIER);
        verify(logger).error(" Cannot find user alert message. Id( " + IDENTIFIER + ")");
        verify(userAlertMessageDAO, never()).update(any(UserAlertVO.class));
    }

    @Test
    public void givenDeleteUserAlertWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final UserAlertVO userAlertVO = initValueObjectNew(initUserAlertVO());
        userAlertVO.setPrimaryId(identifier);
        userAlertVO.getAlertMessage().setPrimaryId(identifierOther);
        when(userAlertMessageDAO.findById(IDENTIFIER)).thenReturn(userAlertVO);
        final DAOException exception = new DAOException(ERROR);
        doThrow(exception).when(userAlertMessageDAO).update(userAlertVO);
        whenNew(Date.class).withNoArguments().thenReturn(currentTime);
        try {
            // Act
            alertMessageBean.deleteUserAlert(IDENTIFIER);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, DAO_EXCEPTION_MSG, exception);
            assertUserAlertVOComplete(userAlertVO, identifier, identifierOther);
            assertSavedValueObjectNew(userAlertVO);
            collector.checkThat(userAlertVO.getIsDeleted(), is(true));
            collector.checkThat(userAlertVO.getDeletedById(), is(USER_ID_1));
            collector.checkThat(userAlertVO.getDeletedTime(), is(currentTime));
            verify(userAlertMessageDAO).findById(IDENTIFIER);
            verify(userAlertMessageDAO).update(userAlertVO);
        }
    }

    @Test
    public void givenGetAlertMessageWhenValidThenExpected() throws Exception {
        // Arrange
        final AlertMessageVO alertMessageVO = initValueObjectNew(initAlertMessageVO());
        alertMessageVO.setPrimaryId(identifierOther);
        when(userAlertMessageDAO.getAlertMessage(IDENTIFIER_OTHER)).thenReturn(alertMessageVO);
        // Act
        final AlertMessageVO result = alertMessageBean.getAlertMessage(IDENTIFIER_OTHER);
        // Assert
        collector.checkThat(result, is(alertMessageVO));
        assertAlertMessageVO(result, identifierOther);
        assertSavedValueObjectNew(result);
        verify(userAlertMessageDAO).getAlertMessage(IDENTIFIER_OTHER);
    }

    @Test
    public void givenGetAlertMessageWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.getAlertMessage(IDENTIFIER_OTHER)).thenThrow(exception);
        try {
            // Act
            alertMessageBean.getAlertMessage(IDENTIFIER_OTHER);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, DAO_EXCEPTION_MSG, exception);
            verify(userAlertMessageDAO).getAlertMessage(IDENTIFIER_OTHER);
        }
    }

    @Test
    public void givenSetMessageReadAndGetDetailsWhenAlertMessageNotNullThenExpected() throws Exception {
        // Arrange
        final UserAlertVO userAlertVO = initValueObject(initUserAlertVO());
        userAlertVO.setPrimaryId(identifier);
        initValueObject(userAlertVO.getAlertMessage());
        userAlertVO.getAlertMessage().setPrimaryId(identifierOther);
        userAlertVO.setIsRead(false);
        when(userAlertMessageDAO.findById(IDENTIFIER)).thenReturn(userAlertVO);
        // Act
        final AlertMessageTO result = alertMessageBean.setMessageReadAndGetDetails(IDENTIFIER);
        // Assert
        assertUserAlertVOComplete(userAlertVO, identifier, identifierOther);
        assertSavedValueObjectModified(userAlertVO);
        verify(userAlertMessageDAO).findById(IDENTIFIER);
        collector.checkThat(result.getUserId(), is(USER_ID_1));
        collector.checkThat(result.getAlertMessage(), is(ALERT_MESSAGE));
        collector.checkThat(result.getAlertType(), is(ALERT_TYPE));
        collector.checkThat(result.getAlertName(), is(ALERT_TYPE.getDisplayName()));
        collector.checkThat(result.getAlertFrom(), is(ALERT_FROM));
        collector.checkThat(result.getAlertSubject(), is(ALERT_SUBJECT));
        collector.checkThat(result.getCreatedByUserId().toLong(), is(USER_ID_1));
        collector.checkThat(result.getCreatedTime(), is(createdTime));
        collector.checkThat(result.getPrimaryId(), is(identifierOther));
    }

    @Test
    public void givenSetMessageReadAndGetDetailsWhenAlertMessageNullThenExpected() throws Exception {
        // Arrange
        final UserAlertVO userAlertVO = initValueObject(initUserAlertVO());
        userAlertVO.setPrimaryId(identifier);
        userAlertVO.setAlertMessage(null);
        userAlertVO.setIsRead(false);
        when(userAlertMessageDAO.findById(IDENTIFIER)).thenReturn(userAlertVO);
        // Act
        final AlertMessageTO result = alertMessageBean.setMessageReadAndGetDetails(IDENTIFIER);
        // Assert
        assertUserAlertVO(userAlertVO, identifier);
        collector.checkThat(userAlertVO.getAlertMessage(), nullValue(AlertMessageVO.class));
        assertSavedValueObjectModified(userAlertVO);
        verify(userAlertMessageDAO).findById(IDENTIFIER);
        collector.checkThat(result.getUserId(), nullValue(Long.class));
        collector.checkThat(result.getAlertMessage(), nullValue(String.class));
        collector.checkThat(result.getAlertType(), nullValue(AlertType.class));
        collector.checkThat(result.getAlertName(), nullValue(String.class));
        collector.checkThat(result.getAlertFrom(), nullValue(String.class));
        collector.checkThat(result.getAlertSubject(), nullValue(String.class));
        collector.checkThat(result.getCreatedByUserId(), nullValue(PrimaryIdentifier.class));
        collector.checkThat(result.getCreatedTime(), nullValue(Date.class));
        collector.checkThat(result.getPrimaryId(), nullValue(PrimaryIdentifier.class));
    }

    @Test
    public void givenSetMessageReadAndGetDetailsWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.findById(IDENTIFIER)).thenThrow(exception);
        try {
            // Act
            alertMessageBean.setMessageReadAndGetDetails(IDENTIFIER);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, DAO_EXCEPTION_MSG, exception);
            verify(userAlertMessageDAO).findById(IDENTIFIER);
        }
    }

    @Test
    public void givenGetUserAlertsWhenAlertMessagesNotEmptyThenExpected() throws Exception {
        // Arrange
        final UserAlertReqTO requestTO = initUserAlertReqTO(userTO);
        final UserAlertVO userAlertVO = initValueObject(initUserAlertVO());
        userAlertVO.setPrimaryId(identifier);
        initValueObject(userAlertVO.getAlertMessage());
        userAlertVO.getAlertMessage().setPrimaryId(identifierOther);
        when(userAlertMessageDAO.getUserAlerts(isA(UserVO.class), isA(UserAlertVO.class), isA(Pagination.class),
                eq(requestTO.getStartDate()), eq(requestTO.getEndDate()))).thenReturn(singletonList(userAlertVO));
        when(userAlertMessageDAO.getUserAlertsCount(isA(UserVO.class), isA(UserAlertVO.class),
                eq(requestTO.getStartDate()), eq(requestTO.getEndDate()))).thenReturn(COUNT);
        // Act
        final UserAlertResTO result = alertMessageBean.getUserAlerts(requestTO);
        // Assert
        collector.checkThat(result.getTotalRecords(), is(COUNT));
        collector.checkThat(result.getMessages().size(), is(1));
        final AlertMessageTO alertMessageTO = result.getMessages().get(0);
        collector.checkThat(alertMessageTO.getPrimaryId(), is(identifier));
        collector.checkThat(alertMessageTO.getIsUrgent(), is(true));
        collector.checkThat(alertMessageTO.getIsRead(), is(true));
        collector.checkThat(alertMessageTO.getAlertSubject(), is(ALERT_SUBJECT));
        collector.checkThat(alertMessageTO.getCategory(), is(ALERT_CATEGORY.toString()));
        collector.checkThat(alertMessageTO.getCreatedTime(), is(createdTime));
        collector.checkThat(alertMessageTO.getAlertName(), is(ALERT_TYPE.toString()));
        collector.checkThat(alertMessageTO.getIsResponseRequested(), is(true));
        verify(userAlertMessageDAO).getUserAlerts((UserVO) objCaptor.capture(), (UserAlertVO) objCaptor.capture(),
                (Pagination) objCaptor.capture(), eq(requestTO.getStartDate()), eq(requestTO.getEndDate()));
        final UserVO capturedUserVO = (UserVO) objCaptor.getAllValues().get(0);
        collector.checkThat(capturedUserVO.getPrimaryId(), is(userId));
        final UserAlertVO capturedUserAlertVO = (UserAlertVO) objCaptor.getAllValues().get(1);
        collector.checkThat(capturedUserAlertVO.getIsRead(), is(true));
        collector.checkThat(capturedUserAlertVO.getIsUrgent(), is(true));
        collector.checkThat(capturedUserAlertVO.getAlertMessage().getCategory(), is(ALERT_CATEGORY));
        final Pagination capturedPagination = (Pagination) objCaptor.getValue();
        assertPagination(capturedPagination);
        verify(userAlertMessageDAO).getUserAlertsCount(capturedUserVO, capturedUserAlertVO, requestTO.getStartDate(),
                requestTO.getEndDate());
    }

    @Test
    public void givenGetUserAlertsWhenAlertMessagesEmptyThenExpected() throws Exception {
        // Arrange
        final UserAlertReqTO requestTO = initUserAlertReqTO(userTO);
        when(userAlertMessageDAO.getUserAlerts(isA(UserVO.class), isA(UserAlertVO.class), isA(Pagination.class),
                eq(requestTO.getStartDate()), eq(requestTO.getEndDate())))
                .thenReturn(Collections.<UserAlertVO>emptyList());
        // Act
        final UserAlertResTO result = alertMessageBean.getUserAlerts(requestTO);
        // Assert
        collector.checkThat(result.getTotalRecords(), is(0));
        collector.checkThat(result.getMessages().isEmpty(), is(true));
        verify(userAlertMessageDAO).getUserAlerts((UserVO) objCaptor.capture(), (UserAlertVO) objCaptor.capture(),
                (Pagination) objCaptor.capture(), eq(requestTO.getStartDate()), eq(requestTO.getEndDate()));
        final UserVO capturedUserVO = (UserVO) objCaptor.getAllValues().get(0);
        collector.checkThat(capturedUserVO.getPrimaryId(), is(userId));
        final UserAlertVO capturedUserAlertVO = (UserAlertVO) objCaptor.getAllValues().get(1);
        collector.checkThat(capturedUserAlertVO.getIsRead(), is(true));
        collector.checkThat(capturedUserAlertVO.getIsUrgent(), is(true));
        collector.checkThat(capturedUserAlertVO.getAlertMessage().getCategory(), is(ALERT_CATEGORY));
        final Pagination capturedPagination = (Pagination) objCaptor.getValue();
        assertPagination(capturedPagination);
        verify(userAlertMessageDAO, never()).getUserAlertsCount(capturedUserVO, capturedUserAlertVO,
                requestTO.getStartDate(), requestTO.getEndDate());
    }

    @Test
    public void givenGetUserAlertsWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final UserAlertReqTO requestTO = initUserAlertReqTO(userTO);
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.getUserAlerts(isA(UserVO.class), isA(UserAlertVO.class), isA(Pagination.class),
                eq(requestTO.getStartDate()), eq(requestTO.getEndDate()))).thenThrow(exception);
        try {
            // Act
            alertMessageBean.getUserAlerts(requestTO);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, DAO_EXCEPTION_MSG, exception);
            verify(userAlertMessageDAO).getUserAlerts((UserVO) objCaptor.capture(), (UserAlertVO) objCaptor.capture(),
                    (Pagination) objCaptor.capture(), eq(requestTO.getStartDate()), eq(requestTO.getEndDate()));
            final UserVO capturedUserVO = (UserVO) objCaptor.getAllValues().get(0);
            collector.checkThat(capturedUserVO.getPrimaryId(), is(userId));
            final UserAlertVO capturedUserAlertVO = (UserAlertVO) objCaptor.getAllValues().get(1);
            collector.checkThat(capturedUserAlertVO.getIsRead(), is(true));
            collector.checkThat(capturedUserAlertVO.getIsUrgent(), is(true));
            collector.checkThat(capturedUserAlertVO.getAlertMessage().getCategory(), is(ALERT_CATEGORY));
            final Pagination capturedPagination = (Pagination) objCaptor.getValue();
            assertPagination(capturedPagination);
            verify(userAlertMessageDAO, never()).getUserAlertsCount(capturedUserVO, capturedUserAlertVO,
                    requestTO.getStartDate(), requestTO.getEndDate());
        }
    }

    @Test
    public void givenSaveAlertMessageWhenValidThenExpected() throws Exception {
        // Arrange
        final AlertMessageTO alertMessageTO = initBaseTransferObject(initAlertMessageTO());
        when(userAlertMessageDAO.saveAlertMessage(isA(AlertMessageVO.class)))
                .then(saveValueObject(AlertMessageVO.class, identifierOther));
        // Act
        final AlertMessageTO result = alertMessageBean.saveAlertMessage(alertMessageTO);
        // Assert
        collector.checkThat(result, is(alertMessageTO));
        assertAlertMessageTO(result, identifierOther);
        assertSavedTransferObjectModified(result);
        verify(userAlertMessageDAO).saveAlertMessage((AlertMessageVO) objCaptor.capture());
        final AlertMessageVO alertMessageVO = (AlertMessageVO) objCaptor.getValue();
        collector.checkThat(alertMessageVO.getCompanyId(), is(BUSINESS_ID));
        collector.checkThat(alertMessageVO.getAlertMessage(), is(ALERT_MESSAGE));
        collector.checkThat(alertMessageVO.getAlertType(), is(ALERT_TYPE));
        collector.checkThat(alertMessageVO.getAlertSubject(), is(ALERT_SUBJECT));
        collector.checkThat(alertMessageVO.getAlertFrom(), is(ALERT_FROM));
        collector.checkThat(alertMessageVO.getIsResponseRequested(), is(true));
        collector.checkThat(alertMessageVO.getCreatedByUserId().toLong(), is(USER_ID_1));
        collector.checkThat(alertMessageVO.getCreatedTime(), is(createdTime));
    }

    @Test
    public void givenSaveAlertMessageWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final AlertMessageTO alertMessageTO = initBaseTransferObject(initAlertMessageTO());
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.saveAlertMessage(isA(AlertMessageVO.class))).thenThrow(exception);
        try {
            // Act
            alertMessageBean.saveAlertMessage(alertMessageTO);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, DAO_EXCEPTION_MSG, exception);
            assertAlertMessageTO(alertMessageTO, null);
            assertSavedTransferObjectModified(alertMessageTO);
            verify(userAlertMessageDAO).saveAlertMessage((AlertMessageVO) objCaptor.capture());
            final AlertMessageVO alertMessageVO = (AlertMessageVO) objCaptor.getValue();
            collector.checkThat(alertMessageVO.getCompanyId(), is(BUSINESS_ID));
            collector.checkThat(alertMessageVO.getAlertMessage(), is(ALERT_MESSAGE));
            collector.checkThat(alertMessageVO.getAlertType(), is(ALERT_TYPE));
            collector.checkThat(alertMessageVO.getAlertSubject(), is(ALERT_SUBJECT));
            collector.checkThat(alertMessageVO.getAlertFrom(), is(ALERT_FROM));
            collector.checkThat(alertMessageVO.getIsResponseRequested(), is(true));
            collector.checkThat(alertMessageVO.getCreatedByUserId().toLong(), is(USER_ID_1));
            collector.checkThat(alertMessageVO.getCreatedTime(), is(createdTime));
        }
    }

    @Test
    public void givenSaveUserAlertWhenValidThenExpected() throws Exception {
        // Arrange
        final UserAlertVO userAlertVO = initValueObjectNew(initUserAlertVO());
        when(userAlertMessageDAO.save(userAlertVO)).then(saveValueObject(UserAlertVO.class, identifier));
        // Act
        alertMessageBean.saveUserAlert(userAlertVO);
        // Assert
        assertUserAlertVOComplete(userAlertVO, identifier, null);
        assertSavedValueObjectNew(userAlertVO);
        verify(userAlertMessageDAO).save(userAlertVO);
    }

    @Test
    public void givenSaveUserAlertWhenUserAlertVONullThenExpected() throws Exception {
        try {
            // Act
            alertMessageBean.saveUserAlert(null);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, "Cannot save null user Alert Message ", null);
            verify(userAlertMessageDAO, never()).save(any(UserAlertVO.class));
        }
    }

    @Test
    public void givenSaveUserAlertWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final UserAlertVO userAlertVO = initValueObjectNew(initUserAlertVO());
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.save(userAlertVO)).thenThrow(exception);
        try {
            // Act
            alertMessageBean.saveUserAlert(userAlertVO);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, DAO_EXCEPTION_MSG, exception);
            assertUserAlertVOComplete(userAlertVO, null, null);
            assertSavedValueObjectNew(userAlertVO);
            verify(userAlertMessageDAO).save(userAlertVO);
        }
    }

    @Test
    public void givenSaveMessageDetailsWhenValidThenExpected() throws Exception {
        // Arrange
        final String messageValueSame = StringUtils.repeat("a", MAX_LENGTH_ALERT_FIELD_VALUE);
        final String messageValueMore = StringUtils.repeat("b", MAX_LENGTH_ALERT_FIELD_VALUE + 1);
        final String messageValueLess = StringUtils.repeat("c", MAX_LENGTH_ALERT_FIELD_VALUE - 1);
        final Map<String, String> messageDetails = new LinkedHashMap<String, String>();
        messageDetails.put(MESSAGE_PROPERTY + 1, MESSAGE_VALUE);
        messageDetails.put(MESSAGE_PROPERTY + 2, null);
        messageDetails.put(MESSAGE_PROPERTY + 3, messageValueSame);
        messageDetails.put(MESSAGE_PROPERTY + 4, messageValueMore);
        messageDetails.put(MESSAGE_PROPERTY + 5, messageValueLess);
        when(userAlertMessageDAO.saveAlertDetail(isA(AlertDetailsVO.class)))
                .then(saveValueObject(AlertDetailsVO.class, identifierOther));
        // Act
        alertMessageBean.saveMessageDetails(IDENTIFIER, messageDetails);
        // Assert
        verify(userAlertMessageDAO, times(4)).saveAlertDetail((AlertDetailsVO) objCaptor.capture());
        final List<AlertDetailsVO> alertDetails = (List<AlertDetailsVO>) objCaptor.getAllValues();
        collector.checkThat(alertDetails.get(0).getPrimaryId(), is(identifierOther));
        collector.checkThat(alertDetails.get(0).getAlertMessageId(), is(IDENTIFIER));
        collector.checkThat(alertDetails.get(0).getProperty(), is(MESSAGE_PROPERTY + 1));
        collector.checkThat(alertDetails.get(0).getValue(), is(MESSAGE_VALUE));
        collector.checkThat(alertDetails.get(1).getPrimaryId(), is(identifierOther));
        collector.checkThat(alertDetails.get(1).getAlertMessageId(), is(IDENTIFIER));
        collector.checkThat(alertDetails.get(1).getProperty(), is(MESSAGE_PROPERTY + 3));
        collector.checkThat(alertDetails.get(1).getValue(), is(messageValueSame));
        collector.checkThat(alertDetails.get(2).getPrimaryId(), is(identifierOther));
        collector.checkThat(alertDetails.get(2).getAlertMessageId(), is(IDENTIFIER));
        collector.checkThat(alertDetails.get(2).getProperty(), is(MESSAGE_PROPERTY + 4));
        collector.checkThat(alertDetails.get(2).getValue(),
                is(messageValueMore.substring(0, MAX_LENGTH_ALERT_FIELD_VALUE)));
        collector.checkThat(alertDetails.get(3).getPrimaryId(), is(identifierOther));
        collector.checkThat(alertDetails.get(3).getAlertMessageId(), is(IDENTIFIER));
        collector.checkThat(alertDetails.get(3).getProperty(), is(MESSAGE_PROPERTY + 5));
        collector.checkThat(alertDetails.get(3).getValue(), is(messageValueLess));
        verify(logger)
                .warn(format("Value null for property: %s for alert message id: %s", MESSAGE_PROPERTY + 2, IDENTIFIER));
    }

    @Test
    public void givenSaveMessageDetailsWhenInvalidAlertMessageIdThenExpected() throws Exception {
        // Arrange
        final String messageValueSame = StringUtils.repeat("a", MAX_LENGTH_ALERT_FIELD_VALUE);
        final String messageValueMore = StringUtils.repeat("b", MAX_LENGTH_ALERT_FIELD_VALUE + 1);
        final String messageValueLess = StringUtils.repeat("c", MAX_LENGTH_ALERT_FIELD_VALUE - 1);
        final Map<String, String> messageDetails = new LinkedHashMap<String, String>();
        messageDetails.put(MESSAGE_PROPERTY + 1, MESSAGE_VALUE);
        messageDetails.put(MESSAGE_PROPERTY + 2, null);
        messageDetails.put(MESSAGE_PROPERTY + 3, messageValueSame);
        messageDetails.put(MESSAGE_PROPERTY + 4, messageValueMore);
        messageDetails.put(MESSAGE_PROPERTY + 5, messageValueLess);
        when(userAlertMessageDAO.saveAlertDetail(isA(AlertDetailsVO.class)))
                .then(saveValueObject(AlertDetailsVO.class, identifierOther));
        // Act
        alertMessageBean.saveMessageDetails(0L, messageDetails);
        // Assert
        verify(userAlertMessageDAO, never()).saveAlertDetail(any(AlertDetailsVO.class));
    }

    @Test
    public void givenSaveMessageDetailsWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final String messageValueSame = StringUtils.repeat("a", MAX_LENGTH_ALERT_FIELD_VALUE);
        final String messageValueMore = StringUtils.repeat("b", MAX_LENGTH_ALERT_FIELD_VALUE + 1);
        final String messageValueLess = StringUtils.repeat("c", MAX_LENGTH_ALERT_FIELD_VALUE - 1);
        final Map<String, String> messageDetails = new LinkedHashMap<String, String>();
        messageDetails.put(MESSAGE_PROPERTY + 1, MESSAGE_VALUE);
        messageDetails.put(MESSAGE_PROPERTY + 2, null);
        messageDetails.put(MESSAGE_PROPERTY + 3, messageValueSame);
        messageDetails.put(MESSAGE_PROPERTY + 4, messageValueMore);
        messageDetails.put(MESSAGE_PROPERTY + 5, messageValueLess);
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.saveAlertDetail(isA(AlertDetailsVO.class))).thenThrow(exception);
        try {
            // Act
            alertMessageBean.saveMessageDetails(IDENTIFIER, messageDetails);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, ERROR, exception);
            verify(userAlertMessageDAO).saveAlertDetail((AlertDetailsVO) objCaptor.capture());
            final AlertDetailsVO alertDetailsVO = (AlertDetailsVO) objCaptor.getValue();
            collector.checkThat(alertDetailsVO.getPrimaryId(), nullValue(PrimaryIdentifier.class));
            collector.checkThat(alertDetailsVO.getAlertMessageId(), is(IDENTIFIER));
            collector.checkThat(alertDetailsVO.getProperty(), is(MESSAGE_PROPERTY + 1));
            collector.checkThat(alertDetailsVO.getValue(), is(MESSAGE_VALUE));
        }
    }

    @Test
    public void givenGetMessageDetailsWhenValidThenExpected() throws Exception {
        // Arrange
        final List<AlertDetailsVO> alertDetails = new ArrayList<AlertDetailsVO>();
        final AlertDetailsVO alertDetailsVO1 = new AlertDetailsVO();
        alertDetailsVO1.setAlertMessageId(IDENTIFIER);
        alertDetailsVO1.setProperty(MESSAGE_PROPERTY + 1);
        alertDetailsVO1.setValue(MESSAGE_VALUE + 1);
        alertDetails.add(alertDetailsVO1);
        final AlertDetailsVO alertDetailsVO2 = new AlertDetailsVO();
        alertDetailsVO2.setAlertMessageId(IDENTIFIER);
        alertDetailsVO2.setProperty(MESSAGE_PROPERTY + 2);
        alertDetailsVO2.setValue(MESSAGE_VALUE + 2);
        alertDetails.add(alertDetailsVO2);
        when(userAlertMessageDAO.getMessageDetails(IDENTIFIER)).thenReturn(alertDetails);
        // Act
        final Map<String, String> result = alertMessageBean.getMessageDetails(IDENTIFIER);
        // Assert
        collector.checkThat(result.size(), is(2));
        collector.checkThat(result.get(MESSAGE_PROPERTY + 1), is(MESSAGE_VALUE + 1));
        collector.checkThat(result.get(MESSAGE_PROPERTY + 2), is(MESSAGE_VALUE + 2));
        verify(userAlertMessageDAO).getMessageDetails(IDENTIFIER);
    }

    @Test
    public void givenGetMessageDetailsWhenInvalidAlertMessageIdThenExpected() throws Exception {
        // Act
        final Map<String, String> result = alertMessageBean.getMessageDetails(0L);
        // Assert
        collector.checkThat(result.isEmpty(), is(true));
        verify(userAlertMessageDAO, never()).getMessageDetails(0L);
    }

    @Test
    public void givenGetMessageDetailsWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.getMessageDetails(IDENTIFIER)).thenThrow(exception);
        try {
            // Act
            alertMessageBean.getMessageDetails(IDENTIFIER);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, ERROR, exception);
            verify(userAlertMessageDAO).getMessageDetails(IDENTIFIER);
        }
    }

    @Test
    public void givenGetUserUnreadAlertsCountWhenValidThenExpected() throws Exception {
        // Arrange
        when(userAlertMessageDAO.getUserUnreadAlertsCount(isA(UserVO.class), eq(ALERT_CATEGORY))).thenReturn(COUNT);
        // Act
        final int result = alertMessageBean.getUserUnreadAlertsCount(userTO, ALERT_CATEGORY);
        // Assert
        collector.checkThat(result, is(COUNT));
        verify(userAlertMessageDAO).getUserUnreadAlertsCount((UserVO) objCaptor.capture(), eq(ALERT_CATEGORY));
        final UserVO capturedUserVO = (UserVO) objCaptor.getValue();
        collector.checkThat(capturedUserVO.getPrimaryId(), is(userId));
    }

    @Test
    public void givenGetUserUnreadAlertsCountWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.getUserUnreadAlertsCount(isA(UserVO.class), eq(ALERT_CATEGORY))).thenThrow(exception);
        try {
            // Act
            alertMessageBean.getUserUnreadAlertsCount(userTO, ALERT_CATEGORY);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, ERROR, exception);
            verify(userAlertMessageDAO).getUserUnreadAlertsCount((UserVO) objCaptor.capture(), eq(ALERT_CATEGORY));
            final UserVO capturedUserVO = (UserVO) objCaptor.getValue();
            collector.checkThat(capturedUserVO.getPrimaryId(), is(userId));
        }
    }

    @Test
    public void givenFetchDashboardItemValueWhenDashboardItemUnreadAlertsThenExpected() throws Exception {
        // Arrange
        final Date endTimeStamp = new Date(CURRENT_TIME + 100L);
        final Date previousThirtyDaysTime = new Date(CURRENT_TIME_OTHER - TimeUnit.DAYS.toMillis(30));
        final Date afterThirtyDaysTime = new Date(CURRENT_TIME_OTHER);
        final Calendar calendar = Calendar.getInstance();
        calendar.setTimeZone(java.util.TimeZone.getTimeZone(TIME_ZONE.getCode()));
        calendar.setTime(currentTimeOther);
        final Date expEndTime = calendar.getTime();
        calendar.add(Calendar.DAY_OF_MONTH, -30);
        final Date expStartTime = calendar.getTime();
        mockStatic(TimeUtils.class);
        final Date[] dates = new Date[2];
        when(TimeUtils.getEndOfDay(isA(Calendar.class))).then(new Answer<Date>() {
            @Override
            public Date answer(final InvocationOnMock invocation) {
                dates[0] = invocation.getArgumentAt(0, Calendar.class).getTime();
                return afterThirtyDaysTime;
            }
        });
        when(TimeUtils.getStartOfDay(isA(Calendar.class))).then(new Answer<Date>() {
            @Override
            public Date answer(final InvocationOnMock invocation) {
                dates[1] = invocation.getArgumentAt(0, Calendar.class).getTime();
                return previousThirtyDaysTime;
            }
        });
        alertMessageBean = spy(alertMessageBean);
        when(userAlertMessageDAO.getUserAlertsCount(isA(UserVO.class), isA(UserAlertVO.class), isA(Date.class),
                isA(Date.class))).thenReturn(COUNT);
        whenNew(Date.class).withNoArguments().thenReturn(currentTime, currentTimeOther, endTimeStamp);
        // Act
        final DashboardItemValue result = alertMessageBean
                .fetchDashboardItemValue(DashboardItem.UNREAD_ALERTS, businessEntityTO, userTO, TIME_ZONE, null);
        // Assert
        collector.checkThat(result, instanceOf(DashboardItemCountValueTO.class));
        collector.checkThat(result.getDashboardItem(), is(DashboardItem.UNREAD_ALERTS));
        collector.checkThat(((DashboardItemCountValueTO) result).getCount(), is((long) COUNT));
        verifyPrivate(alertMessageBean)
                .invoke("getUserUnreadAlertsCountForThirtyDays", objCaptor.capture(), eq(AlertCategory.ALL));
        final UserTO capturedUserTO = (UserTO) objCaptor.getValue();
        collector.checkThat(capturedUserTO.getPrimaryId(), is(userId));
        collector.checkThat(capturedUserTO.getUserName(), is(USER_NAME_3));
        collector.checkThat(capturedUserTO.getUserPreference().getPreferredTimezone(), is(TIME_ZONE.getCode()));
        verify(userAlertMessageDAO).getUserAlertsCount((UserVO) objCaptor.capture(), (UserAlertVO) objCaptor.capture(),
                eq(previousThirtyDaysTime), eq(afterThirtyDaysTime));
        final UserVO capturedUserVO = (UserVO) objCaptor.getAllValues().get(1);
        collector.checkThat(capturedUserVO.getPrimaryId(), is(userId));
        final UserAlertVO capturedUserAlertVO = (UserAlertVO) objCaptor.getAllValues().get(2);
        collector.checkThat(capturedUserAlertVO.getIsRead(), is(false));
        collector.checkThat(capturedUserAlertVO.getIsUrgent(), nullValue(Boolean.class));
        collector.checkThat(capturedUserAlertVO.getAlertMessage().getCategory(), is(AlertCategory.ALL));
        collector.checkThat(dates[0].getTime(), is(expEndTime.getTime()));
        collector.checkThat(dates[1].getTime(), is(expStartTime.getTime()));
        assertNotNull(DashboardItem.UNREAD_ALERTS, "dashboard item", businessEntityTO, "business entity", businessId,
                "business entity's id");
        assertLoggerDebug(
                format("Retrieving value for %s dashboard item took :%d (ms)", DashboardItem.UNREAD_ALERTS, 100L));
    }

    @Test
    public void givenFetchDashboardItemValueWhenDashboardItemUnreadAlertsAndDAOExceptionThenExpected()
            throws Exception {
        // Arrange
        final Date previousThirtyDaysTime = new Date(CURRENT_TIME_OTHER - TimeUnit.DAYS.toMillis(30));
        final Date afterThirtyDaysTime = new Date(CURRENT_TIME_OTHER);
        final Calendar calendar = Calendar.getInstance();
        calendar.setTimeZone(java.util.TimeZone.getTimeZone(TIME_ZONE.getCode()));
        calendar.setTime(currentTimeOther);
        final Date expEndTime = calendar.getTime();
        calendar.add(Calendar.DAY_OF_MONTH, -30);
        final Date expStartTime = calendar.getTime();
        mockStatic(TimeUtils.class);
        final Date[] dates = new Date[2];
        when(TimeUtils.getEndOfDay(isA(Calendar.class))).then(new Answer<Date>() {
            @Override
            public Date answer(final InvocationOnMock invocation) {
                dates[0] = invocation.getArgumentAt(0, Calendar.class).getTime();
                return afterThirtyDaysTime;
            }
        });
        when(TimeUtils.getStartOfDay(isA(Calendar.class))).then(new Answer<Date>() {
            @Override
            public Date answer(final InvocationOnMock invocation) {
                dates[1] = invocation.getArgumentAt(0, Calendar.class).getTime();
                return previousThirtyDaysTime;
            }
        });
        alertMessageBean = spy(alertMessageBean);
        final DAOException exception = new DAOException(ERROR);
        when(userAlertMessageDAO.getUserAlertsCount(isA(UserVO.class), isA(UserAlertVO.class), isA(Date.class),
                isA(Date.class))).thenThrow(exception);
        whenNew(Date.class).withNoArguments().thenReturn(currentTime, currentTimeOther);
        try {
            // Act
            alertMessageBean.fetchDashboardItemValue(DashboardItem.UNREAD_ALERTS, businessEntityTO, userTO,
                    TIME_ZONE, null);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, ERROR, exception);
            verifyPrivate(alertMessageBean)
                    .invoke("getUserUnreadAlertsCountForThirtyDays", objCaptor.capture(), eq(AlertCategory.ALL));
            final UserTO capturedUserTO = (UserTO) objCaptor.getValue();
            collector.checkThat(capturedUserTO.getPrimaryId(), is(userId));
            collector.checkThat(capturedUserTO.getUserName(), is(USER_NAME_3));
            collector.checkThat(capturedUserTO.getUserPreference().getPreferredTimezone(), is(TIME_ZONE.getCode()));
            verify(userAlertMessageDAO).getUserAlertsCount((UserVO) objCaptor.capture(),
                    (UserAlertVO) objCaptor.capture(), eq(previousThirtyDaysTime), eq(afterThirtyDaysTime));
            final UserVO capturedUserVO = (UserVO) objCaptor.getAllValues().get(1);
            collector.checkThat(capturedUserVO.getPrimaryId(), is(userId));
            final UserAlertVO capturedUserAlertVO = (UserAlertVO) objCaptor.getAllValues().get(2);
            collector.checkThat(capturedUserAlertVO.getIsRead(), is(false));
            collector.checkThat(capturedUserAlertVO.getIsUrgent(), nullValue(Boolean.class));
            collector.checkThat(capturedUserAlertVO.getAlertMessage().getCategory(), is(AlertCategory.ALL));
            collector.checkThat(dates[0].getTime(), is(expEndTime.getTime()));
            collector.checkThat(dates[1].getTime(), is(expStartTime.getTime()));
        }
    }

    @Test
    public void givenFetchDashboardItemValueWhenInvalidDashboardItemThenExpected() throws Exception {
        // Arrange
        alertMessageBean = spy(alertMessageBean);
        try {
            // Act
            alertMessageBean.fetchDashboardItemValue(DashboardItem.PRODUCT_CLASSIFICATION_REQUESTS, businessEntityTO,
                    userTO, TIME_ZONE, null);
            fail();
        } catch (final InvalidDataException e) {
            // Assert
            collector.checkThat(e.getMessage(), is(format(
                    "Specified dashboard item (%s) is not supported by this dashboard item value retriever (%s)!",
                    DashboardItem.PRODUCT_CLASSIFICATION_REQUESTS, ChecklistManagementFacade.class)));
            verifyPrivate(alertMessageBean, never())
                    .invoke("getUserUnreadAlertsCountForThirtyDays", any(UserTO.class), eq(AlertCategory.ALL));
        }
    }

    @Test
    public void givenMarkUserMessagesAsReadWhenValidThenExpected() throws Exception {
        // Arrange
        final List<Long> messageIdList = asList(IDENTIFIER, IDENTIFIER_OTHER);
        // Act
        alertMessageBean.markUserMessagesAsRead(messageIdList);
        // Assert
        verify(userAlertMessageDAO).markUserMessagesAsRead(messageIdList);
    }

    @Test
    public void givenMarkUserMessagesAsReadWhenDAOExceptionThenExpected() throws Exception {
        // Arrange
        final List<Long> messageIdList = asList(IDENTIFIER, IDENTIFIER_OTHER);
        final DAOException exception = new DAOException(ERROR);
        doThrow(exception).when(userAlertMessageDAO).markUserMessagesAsRead(messageIdList);
        try {
            // Act
            alertMessageBean.markUserMessagesAsRead(messageIdList);
            fail();
        } catch (final SystemException e) {
            // Assert
            assertBizExceptionNoErrorCode(e, DAO_EXCEPTION_MSG, exception);
            verify(userAlertMessageDAO).markUserMessagesAsRead(messageIdList);
        }
    }

    private void assertUserAlertVOComplete(final UserAlertVO userAlertVO, final PrimaryIdentifier objectId,
            final PrimaryIdentifier messageId) {
        assertUserAlertVO(userAlertVO, objectId);
        assertAlertMessageVO(userAlertVO.getAlertMessage(), messageId);
    }

    private void assertUserAlertVO(final UserAlertVO userAlertVO, final PrimaryIdentifier objectId) {
        collector.checkThat(userAlertVO.getPrimaryId(), is(objectId));
        collector.checkThat(userAlertVO.getUserId(), is(USER_ID_1));
        collector.checkThat(userAlertVO.getIsRead(), is(true));
        collector.checkThat(userAlertVO.getIsUrgent(), is(true));
        collector.checkThat(userAlertVO.getReadTime().getTime(), is(READ_TIME));
    }

    private void assertAlertMessageVO(final AlertMessageVO alertMessageVO, final PrimaryIdentifier objectId) {
        collector.checkThat(alertMessageVO.getPrimaryId(), is(objectId));
        collector.checkThat(alertMessageVO.getAlertType(), is(ALERT_TYPE));
        collector.checkThat(alertMessageVO.getAlertSubject(), is(ALERT_SUBJECT));
        collector.checkThat(alertMessageVO.getAlertFrom(), is(ALERT_FROM));
        collector.checkThat(alertMessageVO.getIsResponseRequested(), is(true));
        collector.checkThat(alertMessageVO.getCategory(), is(ALERT_CATEGORY));
        collector.checkThat(alertMessageVO.getAlertMessage(), is(ALERT_MESSAGE));
        collector.checkThat(alertMessageVO.getCompanyId(), is(BUSINESS_ID));
    }

    private void assertAlertMessageTO(final AlertMessageTO alertMessageTO, final PrimaryIdentifier objectId) {
        collector.checkThat(alertMessageTO.getPrimaryId(), is(objectId));
        collector.checkThat(alertMessageTO.getAlertType(), is(ALERT_TYPE));
        collector.checkThat(alertMessageTO.getAlertSubject(), is(ALERT_SUBJECT));
        collector.checkThat(alertMessageTO.getAlertFrom(), is(ALERT_FROM));
        collector.checkThat(alertMessageTO.getIsResponseRequested(), is(true));
        collector.checkThat(alertMessageTO.getCategory(), is(ALERT_CATEGORY.name()));
        collector.checkThat(alertMessageTO.getAlertName(), is(ALERT_TYPE.getDisplayName()));
        collector.checkThat(alertMessageTO.getAlertMessage(), is(ALERT_MESSAGE));
        collector.checkThat(alertMessageTO.getCompanyId(), is(BUSINESS_ID));
    }

    private void assertPagination(final Pagination pagination) {
        collector.checkThat(pagination.getStartRow(), is(START_ROW));
        collector.checkThat(pagination.getNumberOfRows(), is(NUMBER_OF_ROWS));
        collector.checkThat(pagination.getSortOrder(), is(SORT_ORDER));
        collector.checkThat(pagination.getSortColumn(), is((ColumnSortDefinition) COLUMN_SORT));
    }

    private <B extends BizException, T extends Exception> void assertBizExceptionNoErrorCode(final B exception,
            final String excpMessage, final T cause) {
        collector.checkThat(exception.getErrorCode(), nullValue(ErrorCode.class));
        collector.checkThat(exception.getMessage(), is(excpMessage));
        collector.checkThat(exception.getCause(), is((Throwable) cause));
    }

    private void assertLoggerDebug(final String... messages) {
        for (final String message : messages) {
            logger.debug(message);
        }
    }

    private <T extends ValueObject> void assertSavedValueObjectModified(final T valueObject) {
        collector.checkThat(valueObject.getLastUpdatedTime(), is(lastUpdatedTime));
        collector.checkThat(valueObject.getLastUpdatedByUserId().toLong(), is(USER_ID_2));
        collector.checkThat(valueObject.getLastUpdatedByName().toString(), is(USER_NAME_2));
        collector.checkThat(valueObject.getCreatedTime(), is(createdTime));
        collector.checkThat(valueObject.getCreatedByUserId().toLong(), is(USER_ID_1));
        collector.checkThat(valueObject.getCreatedByName().toString(), is(USER_NAME_1));
    }

    private <T extends ValueObject> void assertSavedValueObjectNew(final T valueObject) {
        collector.checkThat(valueObject.getLastUpdatedTime(), is(createdTime));
        collector.checkThat(valueObject.getLastUpdatedByUserId().toLong(), is(USER_ID_1));
        collector.checkThat(valueObject.getLastUpdatedByName().toString(), is(USER_NAME_1));
        collector.checkThat(valueObject.getCreatedTime(), is(createdTime));
        collector.checkThat(valueObject.getCreatedByUserId().toLong(), is(USER_ID_1));
        collector.checkThat(valueObject.getCreatedByName().toString(), is(USER_NAME_1));
    }

    private <T extends BaseTransferObject> void assertSavedTransferObjectModified(final T transferObject) {
        collector.checkThat(transferObject.getLastUpdatedTime(), is(lastUpdatedTime));
        collector.checkThat(transferObject.getLastUpdatedByUserId().toLong(), is(USER_ID_2));
        collector.checkThat(transferObject.getLastUpdatedByName().toString(), is(USER_NAME_2));
        collector.checkThat(transferObject.getCreatedTime(), is(createdTime));
        collector.checkThat(transferObject.getCreatedByUserId().toLong(), is(USER_ID_1));
        collector.checkThat(transferObject.getCreatedByName().toString(), is(USER_NAME_1));
    }

    private static void assertNotNull(final Object... params) throws MissingRequiredDataException {
        final int numParams = params.length;
        for (int i = 0; numParams > i; i += 2) {
            verifyStatic(Assert.class);
            Assert.assertNotNull(eq(params[i]), eq(format(MISSING_DATA_MSG, params[i + 1])),
                    isNull(ParameterList.class), isNull(ErrorCodeType.class));
        }
    }

    private <T extends ValueObject> T initValueObjectNew(final T valueObject)
            throws MaximumLengthExceededException {
        valueObject.setLastUpdatedTime(createdTime);
        valueObject.setLastUpdatedByUserId(PrimaryIdentifier.valueOf(USER_ID_1));
        valueObject.setLastUpdatedByName(UserName.valueOf(USER_NAME_1));
        valueObject.setCreatedTime(createdTime);
        valueObject.setCreatedByUserId(PrimaryIdentifier.valueOf(USER_ID_1));
        valueObject.setCreatedByName(UserName.valueOf(USER_NAME_1));
        return valueObject;
    }

    private <T extends ValueObject> T initValueObject(final T valueObject)
            throws MaximumLengthExceededException {
        valueObject.setLastUpdatedTime(lastUpdatedTime);
        valueObject.setLastUpdatedByUserId(PrimaryIdentifier.valueOf(USER_ID_2));
        valueObject.setLastUpdatedByName(UserName.valueOf(USER_NAME_2));
        valueObject.setCreatedTime(createdTime);
        valueObject.setCreatedByUserId(PrimaryIdentifier.valueOf(USER_ID_1));
        valueObject.setCreatedByName(UserName.valueOf(USER_NAME_1));
        return valueObject;
    }

    private BusinessEntityTO initBusinessEntityTO() throws MaximumLengthExceededException {
        return initBaseTransferObject(initBusinessEntity(new BusinessEntityTO()));
    }

    private <T extends BusinessEntity> T initBusinessEntity(final T businessEntity)
            throws MaximumLengthExceededException {
        businessEntity.setPrimaryId(businessId);
        businessEntity.setName(BusinessName.valueOf(BUSINESS_NAME));
        businessEntity.setVersion(VERSION);
        businessEntity.setStatus(OPERATIVE_STATUS.name());
        return businessEntity;
    }

    private <T extends BaseTransferObject> T initBaseTransferObject(final T baseTransferObj)
            throws MaximumLengthExceededException {
        baseTransferObj.setLastUpdatedTime(lastUpdatedTime);
        baseTransferObj.setLastUpdatedByUserId(PrimaryIdentifier.valueOf(USER_ID_2));
        baseTransferObj.setLastUpdatedByName(UserName.valueOf(USER_NAME_2));
        baseTransferObj.setCreatedTime(createdTime);
        baseTransferObj.setCreatedByUserId(PrimaryIdentifier.valueOf(USER_ID_1));
        baseTransferObj.setCreatedByName(UserName.valueOf(USER_NAME_1));
        return baseTransferObj;
    }

    private Answer<Void> updateValueObject() {
        return new Answer<Void>() {
            @Override
            public Void answer(final InvocationOnMock invocation) throws MaximumLengthExceededException {
                initValueObject(invocation.getArgumentAt(0, UserAlertVO.class));
                return null;
            }
        };
    }

    private static <T extends ValueObject> Answer<T> saveValueObject(final Class<T> valueObjectClass,
            final PrimaryIdentifier valueObjectId) {
        return new Answer<T>() {
            @Override
            public T answer(final InvocationOnMock invocation) {
                final T valueObject = invocation.getArgumentAt(0, valueObjectClass);
                valueObject.setPrimaryId(valueObjectId);
                return valueObject;
            }
        };
    }

    private static UserAlertVO initUserAlertVO() {
        final UserAlertVO userAlertVO = new UserAlertVO();
        userAlertVO.setAlertMessage(initAlertMessageVO());
        userAlertVO.setUserId(USER_ID_1);
        userAlertVO.setIsRead(true);
        userAlertVO.setIsUrgent(true);
        userAlertVO.setReadTime(new Date(READ_TIME));
        return userAlertVO;
    }

    private static AlertMessageVO initAlertMessageVO() {
        final AlertMessageVO alertMessageVO = new AlertMessageVO();
        alertMessageVO.setAlertType(ALERT_TYPE);
        alertMessageVO.setAlertSubject(ALERT_SUBJECT);
        alertMessageVO.setAlertFrom(ALERT_FROM);
        alertMessageVO.setIsResponseRequested(true);
        alertMessageVO.setCategory(ALERT_CATEGORY);
        alertMessageVO.setAlertMessage(ALERT_MESSAGE);
        alertMessageVO.setCompanyId(BUSINESS_ID);
        return alertMessageVO;
    }

    private static AlertMessageTO initAlertMessageTO() {
        final AlertMessageTO alertMessageTO = new AlertMessageTO();
        alertMessageTO.setAlertType(ALERT_TYPE);
        alertMessageTO.setAlertSubject(ALERT_SUBJECT);
        alertMessageTO.setAlertFrom(ALERT_FROM);
        alertMessageTO.setIsResponseRequested(true);
        alertMessageTO.setCategory(ALERT_CATEGORY.name());
        alertMessageTO.setAlertName(ALERT_TYPE.getDisplayName());
        alertMessageTO.setAlertMessage(ALERT_MESSAGE);
        alertMessageTO.setCompanyId(BUSINESS_ID);
        return alertMessageTO;
    }

    private static UserAlertReqTO initUserAlertReqTO(final UserTO userTO) {
        final UserAlertReqTO requestTO = new UserAlertReqTO();
        requestTO.setAlertCategory(ALERT_CATEGORY);
        requestTO.setIsRead(true);
        requestTO.setIsUrgent(true);
        requestTO.setUser(userTO);
        requestTO.setStartDate(new Date(CURRENT_TIME));
        requestTO.setEndDate(new Date(CURRENT_TIME_OTHER));
        requestTO.setPaginationParameters(new PaginationParameter(START_ROW, NUMBER_OF_ROWS, SORT_ORDER, FIELD_SORT));
        return requestTO;
    }
}
